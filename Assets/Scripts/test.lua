function Start()
	Global.SetNumber("Number", 3.0)
	Global.SetInteger("Integer", 1)
	Global.SetString("MyString", "I am a string")
	Global.SetBoolean("Bool", true)
	Global.SetTable("Table", { x = "test", y = 1, AnEmptyTable = { }, NumArray = { 6,7,8,9,10}})
	Global.SetTable("Mixed-Key-Type-Table", { 1, 2, 3, 4, 5, x = { s = "test"}})
end

-- function Update()
	-- Rigidbody.AddVelocity(0, 0.01, 0)
	-- -- Timer = Timer - deltatime
	-- -- if (Timer <= 0) then
		-- -- Timer = 0.03
		-- -- Entity:Instantiate("cube.prefab", -1.45, -5, 0)	
	-- -- end
	
-- end
function Update()
	Global.SetNumber("DAT FPS", 1/dt)
end

function OnTriggerEnter(other)
	other.Rigidbody.AddVelocity(0, 20, 0)
end