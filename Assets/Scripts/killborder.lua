function OnTriggerEnter(other)
    Log(other.tag)
    other.Destroy()
end
