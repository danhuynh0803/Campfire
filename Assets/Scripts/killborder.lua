function OnTriggerExit(other)
    Log("killborder " .. other.tag)
    other.Destroy()
end
