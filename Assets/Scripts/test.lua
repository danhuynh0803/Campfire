-- SpawnTime = 5
-- Timer = 1
-- s = nil
-- MyTable = {
	-- n = 10,
	-- y = "yolo",
	-- z = function()end,
	-- Mytable2 = {
		-- x = SpawnTime,
		-- y = '10',
		-- z = MyTable,
		-- s = 100,
		-- a = MyTable2,
		-- Mytable3 = {
			-- s = "So many table"
		-- }
	-- },
	-- Timer,
	-- "test",
	-- false,
	-- [false] = 2,
	-- test = Update,
	-- o = nil,
	-- nil
-- }

-- MyTable3 = {
	-- "ad", "dsad","dasd"
-- }

function Start()
	-- Global.SetNumber("x", 1)
	-- y = Global.GetNumber("x")
	-- print(y)
	-- Global.SetInteger("x", 1)
	-- y = Global.GetInteger("x")
	-- print(y)
	-- Global.SetBoolean("x", true)
	-- y = Global.GetBoolean("x")
	-- print(y)
	-- Global.SetString("x", "test")
	-- y = Global.GetString("x")
	-- print(y)
	--Global.SetTable("t", { "x",x = 2, y = "s", o = { n = 2, x = 's', y = false, "ss", s = nil },"ss", "1",{ o = "xx"}, 2.2, 55, nil,[200] = "3.4"})
	--y = Global.GetTable("t")
	-- print(y)
	-- print(y.y)
	-- print(y.o.n)
	-- print(y.o.x)
	-- print(y.o.y)
	-- print(y.o[1])
	-- print(y.o[s])
	-- print(y[1])
	-- print(y[2])
	-- print(y[3])
	-- print(y[4])
	-- print(y[5])
	-- print(y[6])
	-- print(y[4].o)
	-- print(y[7])
	-- print(y[200])
	Global.SetTable("t", { "x",x = 2})
	-- t = Utility.DeseralizeLuaTableX({ 
	-- 	x = 1,
	-- 	y = { true, n = "test"}, 
	-- 	false, 
	-- 	a = {"x","y","z"},
	-- 	"n",
	-- 	{"1.0","0.5","4"},
	-- 	nil,
	-- 	s = { },
	-- 	[100] = "sad", 
	-- 	es = false
	-- })
	-- print(t.s)
	-- print(t.x)
	-- print(t.y)
	-- print(t.y[1])
	-- print(t.y.n)
	-- print(t[1])
	-- print(t[2])
	-- print(t.a[0])
	-- print(t.a[1])
	-- print(t.a[2])
	-- print(t.a[3])
	-- print(t.y[2])
	-- print(t[3][1])
	-- print(t[3][2])
	-- print(t[3][3])
	-- print(t[4])
	-- print(t.s)
	-- print(t[100])
	-- print(t.es)
end

-- function Update()
	-- Rigidbody.AddVelocity(0, 0.01, 0)
	-- -- Timer = Timer - deltatime
	-- -- if (Timer <= 0) then
		-- -- Timer = 0.03
		-- -- Entity:Instantiate("cube.prefab", -1.45, -5, 0)	
	-- -- end
	
-- end

function OnTriggerEnter(other)
	other.Rigidbody.AddVelocity(0, 20, 0)
end