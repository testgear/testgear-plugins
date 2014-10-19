--
-- Shell test script
--

-- tg
device = connect("tcp://127.0.0.1:8000")
device.load("shell")
-- tg
device.shell.command = "touch bla.txt"
status = device.shell.run()
if (status ~= 0) then fail() end
-- tg
device.unload("shell")
disconnect(device)
