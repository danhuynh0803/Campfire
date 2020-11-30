function Start()
    --Transform.SetScale(2, 5, 1)
    --Rigidbody:AddVelocity(0, 200*dt, 0);
end

function Update()
	--Log(deltatime)
end

function OnTriggerEnter(other)
    Log(other.tag)
    --other.Transform.SetScale(2, 2, 2)
    --Log(other.Rigidbody.GetMass())
    other.Rigidbody.AddVelocity(0, 20, 0);
    --Entity:Instantiate("cube.prefab", 0, 10, 0)
end

function OnTriggerStay(other)

end

function OnTriggerExit(other)

end
