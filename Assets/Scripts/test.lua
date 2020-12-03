SpawnTime = 5
Timer = 1

function Update()
	Timer = Timer - deltatime
	if (Timer <= 0) then
		Timer = 0.03
		--Log("Spawn prefab")
		Entity:Instantiate("cube.prefab", -1.45, -5, 0)	
	end
	
end

function OnTriggerEnter(other)
	other.Rigidbody.AddVelocity(0, 20, 0)
	Entity:Instantiate("cube.prefab", 0, 10, 0)	
end