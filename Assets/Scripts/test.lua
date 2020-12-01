function Start()
    
end

function Update()
    Log(1)    
    --Log(deltatime)
end

function OnTriggerEnter(other)
    Entity.Destroy()
    --Log(other.tag)
    --Log(other.tag)
    --other.Transform.SetScale(2, 2, 2)
    --Log(other.Rigidbody.GetMass())
    --other.Rigidbody.AddVelocity(0, 20, 0);
    --Entity:Instantiate("cube.prefab", 0, 10, 0)
end

function OnTriggerStay(other)

end

function OnTriggerExit(other)

end
