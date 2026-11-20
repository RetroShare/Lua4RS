-- External runner only: lua4rs-jsonapi.exe login.lua [list|login|shutdown] [SSL_ID]
-- Configure RETROSHARE_API_URL/USER/PASSWORD for API authentication.
-- For login, set RETROSHARE_PROFILE_PASSWORD separately (the profile password).
-- The server must expose the requested login/control endpoint.
assert(type(rs) ~= "table", "Run login.lua with the standalone lua4rs-jsonapi runner")
local action = arg and arg[1] or "list"
local function api(path, params)
    local result, err = jsonapi.call(path, params)
    assert(result, err)
    return result
end
if action == "list" then
    local result = api("/rsLoginHelper/getLocations")
    for _, loc in ipairs(assert(result.locations, "No locations returned")) do
        print(string.format("%s - %s (%s) / %s", loc.mLocationId, loc.mPgpName, loc.mPgpId, loc.mLocationName))
    end
elseif action == "login" then
    local account = arg[2] or ""
    assert(#account == 32 and account:match("^%x+$"), "Supply the 32-character SSL location ID")
    local password = os.getenv("RETROSHARE_PROFILE_PASSWORD")
    assert(password, "Set RETROSHARE_PROFILE_PASSWORD to the profile password")
    local result = api("/rsLoginHelper/attemptLogin", {account = account, password = password})
    assert(result.retval == 0, "Login failed: " .. tostring(result.retval))
    print("Logged in.")
elseif action == "shutdown" then
    local result = api("/rsControl/rsGlobalShutDown")
    assert(result.retval ~= false, "Shutdown request failed")
    print("Shutdown requested.")
else
    error("Usage: login.lua [list|login|shutdown] [SSL_ID]")
end
