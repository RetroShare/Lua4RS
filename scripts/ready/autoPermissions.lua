-- Auto permissions: decides which friends may use which RetroShare service
-- (chat, file transfer, forums, ...) by rules: by id, group, hidden (Tor/I2P)
-- node, address, profile name or location name.
--
-- The rules describe the complete target state: a friend no rule matches gets
-- each service's default. Exceptions set by hand in the permission matrix are
-- reset too; to keep one, write a rule for it.
--
-- Needs regular runs, e.g. a timer every hour (Automation tab). Until the next
-- run, a new friend has each service's default permissions.
-- Access: changes service permissions, so approve it as "Write" first.
-- Starts with DRY_RUN = true: it only prints what it would change.

-- Settings
local DRY_RUN = true

-- Service names as listed by services.list(); "*" means every service.
local PRESETS = {
    -- No file sharing: no searches, transfers or browsing of shared folders.
    -- Chat, mail, forums, channels and boards keep working.
    noFiles = { turtle = false, ft = false, file_database = false },
    -- Long-offline friends (see buryTheDead): only discovery, so a node that
    -- comes back first gets current addresses and keys.
    graveyard = { ["*"] = false, disc = true },
}

-- In order: a later rule overrides an earlier one for the same service.
-- id: exact SSL id (one location) or PGP id (all locations of a person);
-- group: exact group name; hidden = true: Tor/I2P nodes;
-- address, name (profile name) and location (location name): Lua patterns.
-- Names are chosen by your friends; for a single peer, prefer id.
local RULES = {
    { group = "Strangers",  set = PRESETS.noFiles },
    { hidden = true,        set = PRESETS.noFiles },   -- saves bandwidth over Tor/I2P
    { group = "Graveyard",  set = PRESETS.graveyard },
    -- { id = "SSL_OR_PGP_ID", set = { ["*"] = true } },  -- exception for one peer: put it last
    -- { location = "^tor2$", set = { turtle = true } },
    -- { name = "test",     set = { turtle = false } },
    -- { address = "^ipv6://", set = { rtt = false } },
}

-- Target state ---------------------------------------------------------------

local defaults = {} -- service name -> allowed by default
for _, service in ipairs(services.list() or {}) do
    local p = services.getPermissions(service.id)
    if p then defaults[service.name] = p.defaultAllowed end
end

for _, rule in ipairs(RULES) do
    for name in pairs(rule.set) do
        if name ~= "*" and defaults[name] == nil then error("unknown service in rules: " .. name) end
    end
end

local groupMembers = {} -- group name -> set of PGP ids
for _, group in ipairs(peers.getGroupInfoList() or {}) do
    local members = {}
    for _, pgpId in ipairs(group.peerIds or {}) do members[pgpId] = true end
    groupMembers[group.name] = members
end

local function addressMatches(details, pattern)
    if details.hiddenNode then return (details.hiddenNodeAddress or ""):find(pattern) ~= nil end
    if (details.connectAddr or ""):find(pattern) then return true end
    for _, address in ipairs(details.ipAddressList or {}) do
        if address:find(pattern) then return true end
    end
    return false
end

local function matches(rule, details)
    if rule.id then
        -- case-insensitive: SSL ids come in lowercase, PGP ids in uppercase
        local id = rule.id:lower()
        return id == (details.id or ""):lower() or id == (details.gpg_id or ""):lower()
    end
    if rule.group then return (groupMembers[rule.group] or {})[details.gpg_id] == true end
    if rule.hidden ~= nil then return (details.hiddenNode == true) == rule.hidden end
    if rule.address then return addressMatches(details, rule.address) end
    if rule.name then return (details.name or ""):find(rule.name) ~= nil end
    if rule.location then return (details.location or ""):find(rule.location) ~= nil end
    error("rule needs id, group, hidden, address, name or location")
end

-- wanted[sslId][service] = true/false for every friend and service
local wanted, names = {}, {}
for _, id in ipairs(peers.getFriendList() or {}) do
    local details = peers.getPeerDetails(id)
    if details then
        -- Empty text fields are missing (nil), e.g. a location without a name.
        names[id] = (details.name or id) .. (details.location and " (" .. details.location .. ")" or "")
        local target = {}
        for service, default in pairs(defaults) do target[service] = default end
        for _, rule in ipairs(RULES) do
            if matches(rule, details) then
                local all = rule.set["*"]
                for service in pairs(defaults) do
                    local allowed = rule.set[service]
                    if allowed == nil then allowed = all end
                    if allowed ~= nil then target[service] = allowed end
                end
            end
        end
        wanted[id] = target
    end
end

-- Apply only what differs ----------------------------------------------------

local changes = 0
for id, target in pairs(wanted) do
    for service, allowed in pairs(target) do
        if services.isAllowed(service, id) ~= allowed then
            changes = changes + 1
            local what = (allowed and "allow " or "block ") .. service .. " for " .. names[id]
                         .. (allowed == defaults[service] and " (default)" or "")
            if DRY_RUN then
                rs.print("would " .. what)
            elseif services.setAllowed(service, id, allowed) then
                rs.print(what)
            else
                rs.print("FAILED to " .. what)
            end
        end
    end
end

if changes == 0 then
    rs.print("Auto permissions: everything matches the rules")
elseif DRY_RUN then
    rs.print("Auto permissions: " .. changes .. " change(s) previewed; set DRY_RUN = false to apply them")
end
