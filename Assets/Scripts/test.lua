SpawnTime = 5
Timer = 1
s = nil
MyTable = {
	n = 10,
	y = "yolo",
	z = function()end,
	Mytable2 = {
		x = SpawnTime,
		y = '10',
		z = MyTable,
		s = 100,
		a = MyTable2,
		Mytable3 = {
			s = "So many table"
		}
	},
	Timer,
	"test",
	false,
	[false] = 2,
	test = Update,
	o = nil,
	nil
}


MyTable3 = {
	"ad", "dsad","dasd"
}

function Start()
 	--Rigidbody.AddVelocity(0, 20, 0)
	for n in pairs(_G) do print(n) end
end

function Update()
	Rigidbody.AddVelocity(0, 0.01, 0)
	-- Timer = Timer - deltatime
	-- if (Timer <= 0) then
		-- Timer = 0.03
		-- Entity:Instantiate("cube.prefab", -1.45, -5, 0)	
	-- end
	
end

function OnTriggerEnter(other)
	--other.Rigidbody.AddVelocity(0, 20, 0)
	Entity:Instantiate("cube.prefab", 0, 10, 0)	
end