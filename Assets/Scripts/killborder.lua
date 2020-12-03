function OnTriggerEnter (other)
    Log("killborder " .. other.tag)
    other.Destroy()
end

function OnTriggerExit (other)
    Log("killborder " .. other.tag)
    other.Destroy()
end