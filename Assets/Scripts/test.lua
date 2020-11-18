function Start()

	--for func in transform do
		--print(func)
	
    --tf = Transform.New()
    --tf:SetPosition(2,4,5)
    --print(tf.x)
    --entity:SetScale(2,2,2)
    --entity:SetRotation(90,-40,120)
end

function Update(dt)
	--print(Transform)
	local vel =
	{
		x = 0,
		y = 0,
		z = 0
	}
	
    if (Input:GetButton("KEY_A")) then
		vel.x = -5 * dt		
    elseif (Input:GetButton("KEY_D")) then
		vel.x = 5 * dt
    end
	
	if (Input:GetButton("KEY_W")) then
		vel.y = 5 * dt		
    elseif (Input:GetButton("KEY_S")) then
		vel.y = -5 * dt
    end
	
	Transform:Translate(vel.x, vel.y, vel.z)
--[[
    if (Input:GetButtonDown("KEY_W")) then
        entity:SetPosition(0,5,0)
    elseif (Input:GetButtonDown("KEY_S")) then
        entity:SetPosition(0,-5,0)
    end
]]--
    -- tf = Transform.New()
    -- --print(tf.x,tf.y,tf.z)
    -- x = Tag.PushAOne()
    -- x = x + 1
    -- print(x)
    -- --Transform.SetPosition(tf,1,1,1)
    -- tf:SetPosition(5,4,3) --need to fix the access somehow
    -- print(tf.x)
    --print(tf:GetPosition())
    --print(Transform:GetPosition(tf))
    --print("Update dt = " .. dt)

    --[[ Need to be able to have components tied to entity
    newEntity
    newEntity.transform.SetPosition(0, 0, 0)
    Instantiate(newEntity)
    --]]

    --[[ Be able to reference other entities somehow
    refEntity = FindObjectWithTag("Player")
    refEntity.transform.SetPosition(0, 0, 0)
    --]]

    --Vector2 = [x, y]
    --Vector3 = [x, y, z]
    --Vector4 = [x, y, z, w]

    --refEntity.SetPosition(0, 0, 0)
    --pos.xyz = refEntity.GetPosition(0, 0, 0)
    --entity.SetPosition()
    --tag = entity.GetTag()
    --if (tag == "Player) then:
    --	print("This is player")	
    --end
end
