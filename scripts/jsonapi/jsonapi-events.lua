-- Standalone runner only. Receives the initial registration response followed
-- by event envelopes. Stop with Ctrl+C, or return false from the callback.
local count, err = jsonapi.subscribe(0, function(envelope)
    if envelope.event then
        print(assert(jsonapi.encode(envelope.event)))
    else
        print("Registration response: " .. assert(jsonapi.encode(envelope)))
    end
end)
assert(count, err)
