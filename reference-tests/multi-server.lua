--
-- Multi server test script
--
-- tg
server0_address = "tcp://127.0.0.1:8000"
server1_address = "tcp://127.0.0.1:8001"
-- tg
server0 = connect(server0_address)
server1 = connect(server1_address)
-- tg
server0.load("dummy")
server0.dummy.command0()
-- tg
server1.load("dummy")
server1.dummy.command0()
-- tg
server0.unload("dummy")
disconnect(server0)
-- tg
server1.unload("dummy")
disconnect(server1)
