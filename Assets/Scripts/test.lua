function Start()

end

function Update(dt)
    Log(dt)
end

function OnTriggerEnter(other)
    Log(other.tag)
end
