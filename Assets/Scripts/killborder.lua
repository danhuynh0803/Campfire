function Start()
 	Rigidbody.AddVelocity(0, 20, 0)
	for n in pairs(_G) do print(n) end
end

function OnTriggerEnter (other)
    --Log("killborder " .. other.tag)
    other.Destroy()
	a = other.Transform.GetPosition()
end

function OnTriggerExit (other)
    --Log("killborder " .. other.tag)
    other.Destroy()
end