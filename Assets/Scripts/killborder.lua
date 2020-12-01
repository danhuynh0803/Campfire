function Start()
end

function Update()
end

function OnTriggerEnter(other)
    Log(other.tag)
    other.Destroy()
end

function OnTriggerStay()

end

function OnTriggerExit()

end
