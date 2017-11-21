if args ~= nil and args.searchId ~= nil then
	rs.print("called from event")

	local searchId = args.searchId

	rs.print("result for " .. searchId)
	rs.print(" - " .. args.hash)
	rs.print(" - " .. args.name)
	rs.print(" - " .. args.size)
else
	rs.print("called from gui")
	files.turtleSearch("foobar")
end
