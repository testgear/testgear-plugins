--
-- Audio test script (example)
--

-- tg
device = connect("tcp://127.0.0.1:8000")
device.load("audio")
-- tg
-- Audio configuration
deivce.audio.device = "default"
device.audio.rate = 44100
-- tg
-- Generate 200 Hz tone
device.audio.tone-type = "sine"
device.audio.tone-frequency = 200
device.audio.tone-time = 1
device.audio.generate-tone()
-- tg
-- Generate 400 Hz tone
device.audio.tone-frequency = 400
device.audio.generate-tone()
-- Generate 800 Hz tone
device.audio.tone-frequency = 800
device.audio.generate-tone()
-- Generate 1600 Hz tone
device.audio.tone-frequency = 1600
device.audio.generate-tone()
-- Generate noise
device.audio.tone-type = "noise"
device.audio.generate-tone()
-- Play WAV file
--device.audio.wav-file = "test.wav"
--status = device.audio.play-wav()
--if (status ~= 0) then fail() end
-- tg
device.unload("audio")
disconnect(device)
