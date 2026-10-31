#include "LuaJsonApi.h"
#include "LuaExecutionGuard.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include <cmath>
#include <thread>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

namespace {
char nullValue;
const char* configKey = "Lua4RS.JsonApi.config";
const char* arrayType = "Lua4RS.JsonApi.array";

int failure(lua_State* L, const QString& message, int status = 0)
{
    lua_pushnil(L);
    const QByteArray utf8 = message.toUtf8();
    lua_pushlstring(L, utf8.constData(), utf8.size());
    lua_pushinteger(L, status);
    return 3;
}

QByteArray stringAt(lua_State* L, int index)
{
    size_t size = 0;
    const char* value = lua_tolstring(L, index, &size);
    return value ? QByteArray(value, static_cast<int>(size)) : QByteArray();
}

bool toJson(lua_State* L, int index, QJsonValue& result, QString& error, int depth = 0)
{
    if(!lua_checkstack(L, 8)) { error = "Lua stack exhausted"; return false; }
    if(depth > 32) { error = "JSON nesting exceeds 32 levels (possibly a cyclic table)"; return false; }
    index = lua_absindex(L, index);
    switch(lua_type(L, index))
    {
    case LUA_TNIL: result = QJsonValue(QJsonValue::Null); return true;
    case LUA_TLIGHTUSERDATA:
        if(lua_touserdata(L, index) == &nullValue) { result = QJsonValue(QJsonValue::Null); return true; }
        break;
    case LUA_TBOOLEAN: result = bool(lua_toboolean(L, index)); return true;
    case LUA_TNUMBER:
    {
        const double value = lua_tonumber(L, index);
        if(!std::isfinite(value) || std::abs(value) > 9007199254740991.0)
        { error = "JSON numbers must be finite and within the exact 53-bit range; use strings for large IDs"; return false; }
        result = value;
        return true;
    }
    case LUA_TSTRING: result = QString::fromUtf8(stringAt(L, index)); return true;
    case LUA_TTABLE:
    {
        bool array = false;
        if(lua_getmetatable(L, index))
        {
            luaL_getmetatable(L, arrayType);
            array = lua_rawequal(L, -1, -2);
            lua_pop(L, 2);
        }
        int strings = 0, numbers = 0;
        lua_pushnil(L);
        while(lua_next(L, index))
        {
            if(lua_type(L, -2) == LUA_TSTRING) ++strings;
            else if(lua_type(L, -2) == LUA_TNUMBER && lua_tonumber(L, -2) >= 1 &&
                    lua_tonumber(L, -2) == std::floor(lua_tonumber(L, -2))) ++numbers;
            else { lua_pop(L, 2); error = "JSON table keys must be strings or consecutive positive integers"; return false; }
            lua_pop(L, 1);
        }
        if((strings && numbers) || (array && strings))
        { error = "JSON tables cannot mix object and array keys"; return false; }
        array = array || numbers;
        if(array)
        {
            QJsonArray values;
            for(int i = 1; i <= numbers; ++i)
            {
                lua_rawgeti(L, index, i);
                QJsonValue value;
                const bool valid = !lua_isnil(L, -1) && toJson(L, -1, value, error, depth + 1);
                lua_pop(L, 1);
                if(!valid) { if(error.isEmpty()) error = "JSON arrays cannot have holes; use jsonapi.null"; return false; }
                values.append(value);
            }
            result = values;
        }
        else
        {
            QJsonObject values;
            lua_pushnil(L);
            while(lua_next(L, index))
            {
                QJsonValue value;
                if(!toJson(L, -1, value, error, depth + 1)) { lua_pop(L, 2); return false; }
                values.insert(QString::fromUtf8(stringAt(L, -2)), value);
                lua_pop(L, 1);
            }
            result = values;
        }
        return true;
    }
    default: break;
    }
    error = "Unsupported Lua value in JSON";
    return false;
}

void fromJson(lua_State* L, const QJsonValue& value)
{
    if(value.isNull() || value.isUndefined()) lua_pushlightuserdata(L, &nullValue);
    else if(value.isBool()) lua_pushboolean(L, value.toBool());
    else if(value.isDouble()) lua_pushnumber(L, value.toDouble());
    else if(value.isString())
    {
        const QByteArray text = value.toString().toUtf8();
        lua_pushlstring(L, text.constData(), text.size());
    }
    else if(value.isArray())
    {
        lua_newtable(L);
        luaL_getmetatable(L, arrayType);
        lua_setmetatable(L, -2);
        int index = 1;
        for(const auto& item : value.toArray()) { fromJson(L, item); lua_rawseti(L, -2, index++); }
    }
    else
    {
        lua_newtable(L);
        const QJsonObject object = value.toObject();
        for(auto it = object.begin(); it != object.end(); ++it)
        {
            const QByteArray key = it.key().toUtf8();
            lua_pushlstring(L, key.constData(), key.size());
            fromJson(L, it.value());
            lua_rawset(L, -3);
        }
    }
}

bool validateJson(const QJsonValue& value, QString& error, int depth = 0)
{
    if(depth > 32) { error = "JSON nesting exceeds 32 levels"; return false; }
    if(value.isDouble() && std::abs(value.toDouble()) > 9007199254740991.0)
    { error = "JSON number exceeds the exact 53-bit range"; return false; }
    if(value.isArray())
        for(const auto& child : value.toArray())
            if(!validateJson(child, error, depth + 1)) return false;
    if(value.isObject())
    {
        const auto object = value.toObject();
        for(auto it = object.begin(); it != object.end(); ++it)
            if(!validateJson(it.value(), error, depth + 1)) return false;
    }
    return true;
}

bool parse(const QByteArray& bytes, QJsonValue& value, QString& error)
{
    // Wrapping also permits JSON scalar values on Qt 5.
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson("[" + bytes + "]", &parseError);
    if(parseError.error != QJsonParseError::NoError || !doc.isArray() || doc.array().size() != 1)
    { error = "Invalid JSON: " + parseError.errorString(); return false; }
    value = doc.array().at(0);
    return validateJson(value, error);
}

QByteArray serialize(const QJsonValue& value)
{
    QJsonArray wrapper;
    wrapper.append(value);
    const QByteArray bytes = QJsonDocument(wrapper).toJson(QJsonDocument::Compact);
    return bytes.mid(1, bytes.size() - 2);
}

int encode(lua_State* L)
{
    QJsonValue value;
    QString error;
    if(!toJson(L, 1, value, error)) return failure(L, error);
    const QByteArray bytes = serialize(value);
    lua_pushlstring(L, bytes.constData(), bytes.size());
    return 1;
}

int decode(lua_State* L)
{
    if(!lua_checkstack(L, 128)) return failure(L, "Lua stack exhausted");
    if(lua_type(L, 1) != LUA_TSTRING) return failure(L, "decode expects a JSON string");
    QJsonValue value;
    QString error;
    if(!parse(stringAt(L, 1), value, error)) return failure(L, error);
    fromJson(L, value);
    return 1;
}

int array(lua_State* L)
{
    if(lua_isnoneornil(L, 1)) lua_newtable(L);
    else if(lua_istable(L, 1)) lua_pushvalue(L, 1);
    else return failure(L, "array expects a table");
    luaL_getmetatable(L, arrayType);
    lua_setmetatable(L, -2);
    return 1;
}

int configure(lua_State* L)
{
    if(!lua_istable(L, 1)) return failure(L, "configure expects {url, username, password, timeout_ms}");
    // Copy rather than retaining a mutable user table. Credentials stay in memory.
    lua_newtable(L);
    for(const char* key : {"url", "username", "password", "timeout_ms"})
    {
        lua_getfield(L, 1, key);
        if(!lua_isnil(L, -1) && ((QByteArray(key) == "timeout_ms") ? !lua_isnumber(L, -1) : lua_type(L, -1) != LUA_TSTRING))
        { lua_pop(L, 2); return failure(L, QString("Invalid configuration field: ") + key); }
        lua_setfield(L, -2, key);
    }
    lua_setfield(L, LUA_REGISTRYINDEX, configKey);
    lua_pushboolean(L, true);
    return 1;
}

QByteArray option(lua_State* L, const char* key, const char* environment, const char* fallback = "")
{
    lua_getfield(L, LUA_REGISTRYINDEX, configKey);
    QByteArray result;
    bool specified = false;
    if(lua_istable(L, -1))
    {
        lua_getfield(L, -1, key);
        specified = !lua_isnil(L, -1);
        result = stringAt(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    if(specified) return result;
    result = qgetenv(environment);
    return result.isEmpty() ? QByteArray(fallback) : result;
}

bool connection(lua_State* L, const QString& path, QNetworkRequest& request, int& timeout, QString& error)
{
    QUrl url(QString::fromUtf8(option(L, "url", "RETROSHARE_API_URL", "http://127.0.0.1:9092")));
    if(!url.isValid() || (url.scheme() != "http" && url.scheme() != "https") || url.host().isEmpty() ||
       !url.userInfo().isEmpty() || url.hasQuery() || url.hasFragment() || (url.path() != "" && url.path() != "/"))
    { error = "API URL must be an http(s) origin without credentials, path, query or fragment"; return false; }
    url.setPath(path);
    const QByteArray username = option(L, "username", "RETROSHARE_API_USER");
    const QByteArray password = option(L, "password", "RETROSHARE_API_PASSWORD");
    if(username.contains(':')) { error = "API username must not contain a colon"; return false; }
    bool validTimeout = false;
    timeout = option(L, "timeout_ms", "RETROSHARE_API_TIMEOUT_MS", "10000").toInt(&validTimeout);
    if(!validTimeout || timeout < 1 || timeout > 120000)
    { error = "timeout_ms must be between 1 and 120000"; return false; }
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    if(!username.isEmpty() || !password.isEmpty())
        request.setRawHeader("Authorization", "Basic " + (username + ':' + password).toBase64());
    return true;
}

int call(lua_State* L)
{
    if(!lua_checkstack(L, 128)) return failure(L, "Lua stack exhausted");
    if(lua_type(L, 1) != LUA_TSTRING) return failure(L, "call expects an endpoint and an optional parameter table or JSON object string");
    const QString path = QString::fromUtf8(stringAt(L, 1));
    if(!path.startsWith('/') || path.startsWith("//") || path.contains('?') || path.contains('#') || path.contains("..") || path.contains('\\'))
        return failure(L, "Endpoint must be an absolute API path such as /rsPeers/getFriendList");
    if(path == "/rsEvents/registerEventsHandler")
        return failure(L, "This endpoint streams events; jsonapi.call supports finite JSON responses only");
    QJsonValue payload(QJsonObject{});
    QString error;
    if(lua_type(L, 2) == LUA_TSTRING)
    {
        if(!parse(stringAt(L, 2), payload, error)) return failure(L, error);
    }
    else if(!lua_isnoneornil(L, 2) && !toJson(L, 2, payload, error)) return failure(L, error);
    if(!payload.isObject()) return failure(L, "API parameters must be a JSON object");
    QNetworkRequest request;
    int timeout = 0;
    if(!connection(L, path, request, timeout, error)) return failure(L, error);
    request.setRawHeader("Accept", "application/json");
    const QByteArray body = serialize(payload);
    QByteArray response;
    int status = 0;
    timeout = LuaExecutionGuard::remainingMilliseconds(timeout);
    const auto cancel = LuaExecutionGuard::cancellationFlag();
    // A private event loop keeps synchronous Lua calls from re-entering the GUI.
    std::thread worker([&]() {
        QNetworkAccessManager manager;
        QNetworkReply* reply = manager.post(request, body);
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        QTimer cancellationTimer;
        if(cancel)
        {
            QObject::connect(&cancellationTimer, &QTimer::timeout, [&]() {
                if(cancel->load()) { error = "JSON API request cancelled"; reply->abort(); }
            });
            cancellationTimer.start(50);
        }
        QObject::connect(&timer, &QTimer::timeout, [&]() { error = "JSON API request timed out"; reply->abort(); });
        QObject::connect(reply, &QNetworkReply::readyRead, [&]() {
            response += reply->readAll();
            if(response.size() > 8 * 1024 * 1024) { error = "JSON API response exceeds 8 MiB"; reply->abort(); }
        });
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        timer.start(timeout);
        loop.exec();
        status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        response += reply->readAll();
        if(error.isEmpty() && (status < 200 || status >= 300))
            error = status ? QString("JSON API returned HTTP %1").arg(status) : "JSON API connection failed: " + reply->errorString();
        if(error.isEmpty() && reply->error() != QNetworkReply::NoError) error = reply->errorString();
        delete reply;
    });
    worker.join();
    if(!error.isEmpty()) return failure(L, error, status);
    QJsonValue value;
    if(!parse(response, value, error)) return failure(L, error, status);
    fromJson(L, value);
    lua_pushnil(L);
    lua_pushinteger(L, status);
    return 3;
}

int subscribe(lua_State* L)
{
    if(!lua_toboolean(L, lua_upvalueindex(1)))
        return failure(L, "subscribe is available in the standalone runner; use existing Lua4RS triggers inside the GUI");
    if(!lua_checkstack(L, 128)) return failure(L, "Lua stack exhausted");
    if(lua_type(L, 1) != LUA_TNUMBER || lua_tonumber(L, 1) < 0 ||
       lua_tonumber(L, 1) > 4294967295.0 || lua_tonumber(L, 1) != std::floor(lua_tonumber(L, 1)) || !lua_isfunction(L, 2))
        return failure(L, "subscribe expects an integer event type (0 for all) and a callback");
    const QByteArray body = "{\"eventType\":" + QByteArray::number(lua_tonumber(L, 1), 'f', 0) + "}";
    QNetworkRequest request;
    int timeout = 0;
    QString error;
    if(!connection(L, "/rsEvents/registerEventsHandler", request, timeout, error)) return failure(L, error);
    request.setRawHeader("Accept", "text/event-stream");
    int status = 0, count = 0;
    bool stopped = false;
    std::thread worker([&]() {
        QNetworkAccessManager manager;
        QNetworkReply* reply = manager.post(request, body);
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        QByteArray pending, data;
        QObject::connect(&timer, &QTimer::timeout, [&]() { error = "JSON API event connection timed out"; reply->abort(); });
        QObject::connect(reply, &QNetworkReply::metaDataChanged, [&]() {
            status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if(status == 200 && reply->header(QNetworkRequest::ContentTypeHeader).toByteArray().startsWith("text/event-stream"))
                timer.stop(); // A quiet event stream is healthy; only establishment has a deadline.
            else { error = QString("Expected event stream, received HTTP %1").arg(status); reply->abort(); }
        });
        QObject::connect(reply, &QNetworkReply::readyRead, [&]() {
            if(stopped || !error.isEmpty()) return;
            pending += reply->readAll();
            if(pending.size() + data.size() > 8 * 1024 * 1024)
            { error = "JSON API event exceeds 8 MiB"; reply->abort(); return; }
            int newline = -1;
            while((newline = pending.indexOf('\n')) >= 0)
            {
                QByteArray line = pending.left(newline);
                pending.remove(0, newline + 1);
                if(line.endsWith('\r')) line.chop(1);
                if(line.startsWith("data:"))
                {
                    line.remove(0, 5);
                    if(line.startsWith(' ')) line.remove(0, 1);
                    data += line + '\n';
                }
                else if(line.isEmpty() && !data.isEmpty())
                {
                    QJsonValue value;
                    if(!parse(data, value, error)) { reply->abort(); return; }
                    data.clear();
                    // Caller waits in join(); this is the sole thread accessing L.
                    lua_pushvalue(L, 2);
                    fromJson(L, value);
                    if(lua_pcall(L, 1, 1, 0) != LUA_OK)
                    {
                        error = "Event callback failed: " + QString::fromUtf8(stringAt(L, -1));
                        lua_pop(L, 1);
                        reply->abort();
                        return;
                    }
                    stopped = lua_isboolean(L, -1) && !lua_toboolean(L, -1);
                    lua_pop(L, 1);
                    ++count;
                    if(stopped) { reply->abort(); return; }
                }
            }
        });
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        timer.start(timeout);
        loop.exec();
        if(!stopped && error.isEmpty()) error = "JSON API event stream disconnected";
        delete reply;
    });
    worker.join();
    if(!error.isEmpty()) return failure(L, error, status);
    lua_pushinteger(L, count);
    lua_pushnil(L);
    lua_pushinteger(L, status);
    return 3;
}
}

void registerLuaJsonApi(lua_State* L, bool standalone)
{
    luaL_newmetatable(L, arrayType);
    lua_pop(L, 1);
    const luaL_Reg functions[] = {
        {"configure", configure}, {"call", call}, {"encode", encode},
        {"decode", decode}, {"array", array}, {nullptr, nullptr}
    };
    luaL_newlib(L, functions);
    lua_pushlightuserdata(L, &nullValue);
    lua_setfield(L, -2, "null");
    lua_pushboolean(L, standalone);
    lua_pushcclosure(L, subscribe, 1);
    lua_setfield(L, -2, "subscribe");
    lua_setglobal(L, "jsonapi");
}
