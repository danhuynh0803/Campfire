function OnTriggerExit(other)
    Log("killborder " .. other.tag)
    other.Destroy()
	a = other.Transform.GetPosition()
end
