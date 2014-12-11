--
-- USB test script (example)
--
-- http://testgear.io
--

-- tg
device = connect("tcp://127.0.0.1:8000")
device.load("usb")
-- tg
-- Configure usb test
device.usb.vid = 0x221a
device.usb.pid = 0x100
device.usb.ep_in = 2
device.usb.ep_out = 4
device.usb.mode = "loopback"
device.usb.buffer_size = 512
device.usb.verify = 1
-- tg
status = device.usb.start()
if (status ~= 0) then fail() end
-- tg
-- Run USB loopback test for 10 s
sleep(10)
-- tg
status = device.usb.stop()
if (status ~= 0) then fail() end
-- tg
-- Make sure bandwith rate is tested above a certain threshold
if (device.usb.rate < 100000) then fail() end
-- tg
-- Check for TX errors
if (device.usb.tx_errors > 0) then fail() end
-- tg
-- Check for RX errors
if (device.usb.rx_errors > 0) then fail() end
-- tg
-- Check for verification errors
if (device.usb.verify_errors > 0) then fail() end
-- tg
device.unload("usb")
disconnect(device)
