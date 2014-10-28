--
-- Audio test script (example)
--
-- http://testgear.io
--

-- tg
device = connect("tcp://127.0.0.1:8000")
device.load("audio")
-- tg
-- Audio configuration
device.audio.device = "default"
device.audio.rate = 44100
device.audio.mode = "stereo"
-- tg
-- Generate 200 Hz sine tone
device.audio.tone_type = "sine"
device.audio.tone_frequency = 200
device.audio.tone_time = 2
device.audio.generate_tone()
-- tg
-- Generate 400 Hz sine tone
device.audio.tone_frequency = 400
device.audio.generate_tone()
-- tg
-- Generate 800 Hz sine tone
device.audio.tone_frequency = 800
device.audio.generate_tone()
-- tg
-- Generate 1600 Hz sine tone
device.audio.tone_frequency = 1600
device.audio.generate_tone()
-- tg
-- Generate noise
device.audio.tone_type = "noise"
device.audio.generate_tone()
-- tg
-- Play WAV file
--device.audio.wav_file = "test.wav"
--status = device.audio.play_wav()
--if (status ~= 0) then fail() end
-- tg
device.unload("audio")
disconnect(device)
