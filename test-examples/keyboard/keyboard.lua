--
-- Keyboard input test script (example)
--

-- tg
device = connect("tcp://127.0.0.1:8000")
device.load("keyboard")
-- tg
device.keyboard.device = "/dev/input/by-id/usb-04f3_0103-event-kbd"
device.keyboard.wait_key_event()
-- tg
print("Key " .. device.keyboard.key .. " pressed")
-- tg
device.unload("keyboard")
disconnect(device)
