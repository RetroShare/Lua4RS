#include <retroshare/rsmail.h>

#include <cstring>
#include <set>

#include "LuaCore.h"
#include "LuaToRS.h"
#include "LuaToRSIds.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace {
using namespace Rs::Mail;

const char* addressType(const MsgAddress& a)
{
    switch(a.type())
    {
    case MsgAddress::MSG_ADDRESS_TYPE_RSGXSID:  return "identity";
    case MsgAddress::MSG_ADDRESS_TYPE_RSPEERID: return "node";
    case MsgAddress::MSG_ADDRESS_TYPE_PLAIN:    return "email";
    default: break;
    }
    return "unknown";
}

const char* addressMode(const MsgAddress& a)
{
    switch(a.mode())
    {
    case MsgAddress::MSG_ADDRESS_MODE_TO:  return "to";
    case MsgAddress::MSG_ADDRESS_MODE_CC:  return "cc";
    case MsgAddress::MSG_ADDRESS_MODE_BCC: return "bcc";
    default: break;
    }
    return "unknown";
}

void setString(lua_State* L, const char* key, const std::string& value)
{
    lua_pushlstring(L, value.data(), value.size());
    lua_setfield(L, -2, key);
}

void setBool(lua_State* L, const char* key, bool value)
{
    lua_pushboolean(L, value);
    lua_setfield(L, -2, key);
}

void pushAddress(lua_State* L, const MsgAddress& a)
{
    lua_createtable(L, 0, 3);
    setString(L, "address", a.toStdString());
    setString(L, "type", addressType(a));
    setString(L, "mode", addressMode(a));
}

void pushDestinations(lua_State* L, const std::set<MsgAddress>& destinations)
{
    lua_createtable(L, int(destinations.size()), 0);
    int i = 1;
    for(const MsgAddress& a : destinations) { pushAddress(L, a); lua_rawseti(L, -2, i++); }
}

// Fields shared by summaries and full messages. Table on top of the stack.
void setCommon(lua_State* L, const std::string& id, const MsgAddress& from, unsigned flags,
               const std::string& title, long long ts, int files)
{
    setString(L, "id", id);
    setString(L, "from", from.toStdString());
    setString(L, "fromType", addressType(from));
    setString(L, "title", title);
    lua_pushinteger(L, lua_Integer(ts)); lua_setfield(L, -2, "time");
    lua_pushinteger(L, files);           lua_setfield(L, -2, "files");
    setBool(L, "unread", flags & (RS_MSG_NEW | RS_MSG_UNREAD_BY_USER));
    setBool(L, "new", flags & RS_MSG_NEW);
    setBool(L, "outgoing", flags & RS_MSG_OUTGOING);
    setBool(L, "trash", flags & RS_MSG_TRASH);
    setBool(L, "distant", flags & RS_MSG_DISTANT);
    setBool(L, "signatureChecked", flags & RS_MSG_SIGNATURE_CHECKS);
}

bool boxByName(const char* name, BoxName& box)
{
    static const struct { const char* name; BoxName box; } boxes[] = {
        { "inbox", BoxName::BOX_INBOX }, { "outbox", BoxName::BOX_OUTBOX }, { "drafts", BoxName::BOX_DRAFTS },
        { "sent", BoxName::BOX_SENT }, { "trash", BoxName::BOX_TRASH }, { "all", BoxName::BOX_ALL } };
    for(const auto& b : boxes) if(std::strcmp(name, b.name) == 0) { box = b.box; return true; }
    return false;
}
}

extern "C" {

    // mail.getMessageSummaries(["inbox"|"sent"|"outbox"|"drafts"|"trash"|"all"]) -> array (no bodies)
    int mail_getMessageSummaries(lua_State* L)
    {
        BoxName box = BoxName::BOX_INBOX;
        if(!lua_isnoneornil(L, 1) && !boxByName(luaL_checkstring(L, 1), box))
            return luaL_argerror(L, 1, "box must be inbox, outbox, drafts, sent, trash or all");
        std::list<MsgInfoSummary> list;
        if(!rsMail->getMessageSummaries(box, list)) return 0;
        lua_createtable(L, int(list.size()), 0);
        int i = 1;
        for(const MsgInfoSummary& m : list)
        {
            lua_createtable(L, 0, 14);
            setCommon(L, m.msgId, m.from, m.msgflags, m.title, m.ts, m.count);
            pushDestinations(L, m.destinations);
            lua_setfield(L, -2, "destinations");
            lua_rawseti(L, -2, i++);
        }
        return 1;
    }

    // mail.getMessage("MSG_ID") -> table with body and files, or nil
    int mail_getMessage(lua_State* L)
    {
        size_t length = 0;
        const char* id = luaL_checklstring(L, 1, &length);
        MessageInfo m;
        if(!rsMail->getMessage(std::string(id, length), m)) return 0;
        lua_createtable(L, 0, 16);
        setCommon(L, m.msgId, m.from, m.msgflags, m.title, m.ts, m.count);
        setString(L, "body", m.msg);
        pushDestinations(L, m.destinations);
        lua_setfield(L, -2, "destinations");
        lua_createtable(L, int(m.files.size()), 0);
        int i = 1;
        for(const FileInfo& f : m.files)
        {
            lua_createtable(L, 0, 3);
            setString(L, "name", f.fname);
            setString(L, "hash", f.hash.toStdString());
            lua_pushinteger(L, lua_Integer(f.size)); lua_setfield(L, -2, "size");
            lua_rawseti(L, -2, i++);
        }
        lua_setfield(L, -2, "attachments");
        return 1;
    }

    // mail.sendMail("FROM_GXS_ID", "TO_GXS_ID" or {ids...}, "subject", "body") -> number sent[, error]
    int mail_sendMail(lua_State* L)
    {
        RsGxsId from;
        if(!luaIdArg(L, 1, from)) return luaL_argerror(L, 1, "invalid identity id (use one of identity.getOwnSignedIds())");
        size_t subjectLength = 0, bodyLength = 0;
        const char* subject = luaL_checklstring(L, 3, &subjectLength);
        const char* body = luaL_checklstring(L, 4, &bodyLength);

        // Recipients first, so argument errors are raised before C++ objects exist.
        int recipients = 0;
        if(lua_type(L, 2) == LUA_TSTRING) recipients = 1;
        else if(lua_type(L, 2) == LUA_TTABLE) recipients = int(lua_rawlen(L, 2));
        else return luaL_argerror(L, 2, "recipient id or list of ids expected");
        if(recipients == 0) return luaL_argerror(L, 2, "no recipients");
        for(int i = 1; i <= recipients; ++i)
        {
            if(lua_type(L, 2) == LUA_TTABLE) lua_rawgeti(L, 2, i); else lua_pushvalue(L, 2);
            RsGxsId to;
            const bool valid = lua_type(L, -1) == LUA_TSTRING && luaIdArg(L, lua_gettop(L), to);
            lua_pop(L, 1);
            if(!valid) return luaL_argerror(L, 2, "invalid recipient identity id");
        }

        uint32_t sent = 0;
        bool failed = false;
        {
            std::set<RsGxsId> to;
            for(int i = 1; i <= recipients; ++i)
            {
                if(lua_type(L, 2) == LUA_TTABLE) lua_rawgeti(L, 2, i); else lua_pushvalue(L, 2);
                to.insert(RsGxsId(std::string(lua_tostring(L, -1))));
                lua_pop(L, 1);
            }
            std::string error;
            std::set<RsMailIdRecipientIdPair> tracking;
            sent = rsMail->sendMail(from, std::string(subject, subjectLength), std::string(body, bodyLength),
                                    to, std::set<RsGxsId>(), std::set<RsGxsId>(), std::vector<FileInfo>(),
                                    tracking, error);
            lua_pushinteger(L, sent);
            if(!error.empty()) { failed = true; lua_pushlstring(L, error.data(), error.size()); }
        }
        return failed ? 2 : 1;
    }

    // mail.markRead("MSG_ID"[, read = true]) -> true on success
    int mail_markRead(lua_State* L)
    {
        size_t length = 0;
        const char* id = luaL_checklstring(L, 1, &length);
        const bool read = lua_isnoneornil(L, 2) || lua_toboolean(L, 2);
        bool ok;
        { ok = rsMail->MessageRead(std::string(id, length), !read); }
        lua_pushboolean(L, ok);
        return 1;
    }

    // mail.trashMessage("MSG_ID") -> true on success (can be restored from the trash)
    int mail_trashMessage(lua_State* L)
    {
        size_t length = 0;
        const char* id = luaL_checklstring(L, 1, &length);
        bool ok;
        { ok = rsMail->MessageToTrash(std::string(id, length), true); }
        lua_pushboolean(L, ok);
        return 1;
    }

    // mail.deleteMessage("MSG_ID") -> true on success (permanent)
    int mail_deleteMessage(lua_State* L)
    {
        size_t length = 0;
        const char* id = luaL_checklstring(L, 1, &length);
        bool ok;
        { ok = rsMail->MessageDelete(std::string(id, length)); }
        lua_pushboolean(L, ok);
        return 1;
    }
}
