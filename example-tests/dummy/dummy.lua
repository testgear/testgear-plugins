--
-- Dummy test script
--

-- To run test script simply do (in src/ dir):
-- ./testgearctl ../tests/dummy.lua
--

-- tg
-- Dummy test case
device = connect("tcp://127.0.0.1:8000")
device.list_plugins()
device.load("dummy")
-- tg
device.dummy.char0 = 42
if (device.dummy.char0 ~= 42) then fail() end
-- tg
device.dummy.short0 = 4242
if (device.dummy.short0 ~= 4242) then fail() end
-- tg
device.dummy.int0 = 424242
if (device.dummy.int0 ~= 424242) then fail() end
-- tg
device.dummy.long0 = 4242424242
if (device.dummy.long0 ~= 4242424242) then fail() end
-- tg
device.dummy.float0 = 4.424242
if (device.dummy.float0 ~= 4.424242) then fail() end
-- tg
device.dummy.double0 = 42.4242424242
if (device.dummy.double0 ~= 42.4242424242) then fail() end
-- tg
device.dummy.string0 = "Hello world!"
if (device.dummy.string0 ~= "Hello world!") then fail() end
-- tg
device.dummy.command0()
-- tg
device.unload("dummy")
disconnect(device)
