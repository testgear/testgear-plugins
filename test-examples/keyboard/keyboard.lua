--
-- Keyboard input test script (example)
--

-- tg
device = connect("tcp://127.0.0.1:8000")
device.load("keyboard")

-- tg
device.keyboard.device = "/dev/input/by-id/usb-04f3_0103-event-kbd"
-- tg

-- Keys to test
keys = {30, 31, 32, 33, 34, 35}

-- tg
::input_loop::

print("Waiting for keyboard input event...")
device.keyboard.wait_key_event()
key = device.keyboard.key
print("Key " .. key .. " pressed")

-- Test if key has been pressed before
for i, v in ipairs(keys) do
    if (v == key) then
        table.remove(keys, i)
    end
end

-- Repeat until all keys have been pressed
if (table.maxn(keys) ~= 0) then
    goto input_loop
end

-- tg
device.unload("keyboard")
disconnect(device)
