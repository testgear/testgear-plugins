--
-- Framebuffer test script (example)
--

-- tg
device = connect("tcp://127.0.0.1:8000")
device.load("fb")
-- tg
-- Display configuration
device.fb.device = "/dev/fb0"
device.fb.depth = 32
device.fb.set_depth()
device.fb.xres = 1920
device.fb.yres = 1080
device.fb.set_resolution()
-- tg
-- Draw pattern 0
device.fb.pattern = 0
device.fb.draw_pattern()
sleep(2)
-- Draw pattern 1
device.fb.pattern = 1
device.fb.draw_pattern()
sleep(2)
-- Draw pattern 2
device.fb.pattern = 2
device.fb.draw_pattern()
sleep(2)
-- Draw pattern 3
device.fb.pattern = 3
device.fb.draw_pattern()
sleep(2)
-- Draw pattern 4
device.fb.pattern = 4
device.fb.draw_pattern()
sleep(2)
-- Draw image (TBD)
--device.fb.filename = "test.bmp"
--device.fb.show_image()
--sleep(2)
-- tg
device.unload("fb")
disconnect(device)
