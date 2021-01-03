function Start()
 	Rigidbody.AddVelocity(0, 20, 0)
end

function OnTriggerEnter (other)
    --Log("killborder " .. other.Tag.GetTag())
    other.Destroy()
	a = other.Transform.GetPosition()
end

function OnTriggerExit (other)
    --Log("killborder " .. other.Tag.GetTag())
    other.Destroy()
end