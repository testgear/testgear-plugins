--
-- Lua test script
--

-- To run this test script simply do (in src/ dir):
-- ./testgearctl ../tests/lua-tests.lua
--
-- tg
-- Print stuff to screen
print("GREETINGS PROFESSOR FALKEN")
-- tg
-- Ask a question
local answer
repeat
   io.write("SHALL WE PLAY A GAME (y/n)? ")
   io.flush()
   answer=io.read()
until answer=="y" or answer=="n"
if answer=="n" then fail() end
-- tg
-- Force lua error
-
-- tg
-- Wait for user input to continue
io.write("Press <Enter> to continue...")
io.read()
-- tg
-- Force Test Gear failure
fail()
