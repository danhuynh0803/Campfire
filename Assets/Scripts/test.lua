SpawnTime = 5
Timer = 1

function Update()
	Timer = Timer - deltatime;
	if (Timer <= 0) then
		--Timer = 5
		Log("Spawn prefab")
		Entity:Instantiate("cube.prefab", 0, 20, 0);	
	end
	
end

function OnTriggerEnter(other)
	other.Rigidbody.AddVelocity(0, 20, 0);
	Entity:Instantiate("cube.prefab", 0, 10, 0);	
end