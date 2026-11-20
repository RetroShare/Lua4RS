-- Chat bot: answers chat messages that match simple rules, e.g. "ping" -> "pong".
--
-- Trigger: the event "A chat message arrives" (Automation tab), then Save.
-- Access: sends chat messages, so approve it as "Write" first.
-- Chat messages are written by other people: the bot only compares them with
-- the rules below, it never runs them.

-- Settings
local PREFIX = "[Lua4RS]: "          -- put in front of every answer
local LOBBIES_ONLY = true            -- false: also answer private chats
local RULES = {
    -- match: text to look for; whole = true: the message must be exactly that
    -- case = true: case-sensitive; answer: reply, {nick} = sender's nickname
    { match = "ping",   whole = true,  answer = "pong" },
    { match = "test",   whole = true,  answer = "@{nick}: test back :P" },
    { match = "lua4rs", whole = false, answer = "\\o/" },
    { match = "!info",  whole = true,  case = true,
      answer = "I'm a Lua script: https://github.com/sehraf/Lua4RS" },
}
local RULES_COMMAND = "!rules"       -- lists the rules; nil to disable

if type(args) ~= "table" or type(args.msg) ~= "string" or type(args.chatid) ~= "string" then
    rs.print("Chat bot: waiting for chat messages. Enable the trigger \"A chat message arrives\" and Save.")
    return
end
if LOBBIES_ONLY and not args.chatid:match("^L") then return end

local msg = args.msg:match("^%s*(.-)%s*$")
if msg == "" or #msg > 150 or msg:sub(1, #PREFIX) == PREFIX then return end -- ignore own answers
local nick = args.nick or "someone"

local function reply(text)
    if not chat.sendChat(args.chatid, PREFIX .. text) then
        rs.print("Chat bot: could not send to " .. args.chatid)
    end
end

for _, rule in ipairs(RULES) do
    local text, pattern = msg, rule.match
    if not rule.case then text, pattern = text:lower(), pattern:lower() end
    local hit
    if rule.whole then hit = text == pattern else hit = text:find(pattern, 1, true) ~= nil end
    if hit then
        rs.print("Chat bot: \"" .. rule.match .. "\" from " .. nick)
        reply((rule.answer:gsub("{nick}", function() return nick end)))
    end
end

if RULES_COMMAND and msg == RULES_COMMAND then
    local names = {}
    for _, rule in ipairs(RULES) do names[#names + 1] = rule.match end
    reply("I answer to: " .. table.concat(names, ", "))
end
