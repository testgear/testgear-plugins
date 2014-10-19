--
-- CPU test script
--

-- tg
device = connect("tcp://127.0.0.1:8000")
device.load("cpu")
-- tg
device.cpu.threads = 8
status = device.cpu.start()
if (status ~= 0) then fail() end
-- tg
sleep(120)
-- tg
status = device.cpu.stop()
if (status ~= 0) then fail() end
-- tg
device.unload("cpu")
disconnect(device)
