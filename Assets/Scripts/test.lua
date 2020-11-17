function Start()
	tf = Transform.New()
	tf:SetPosition(2,4,5)
	print(tf.x)
end

function Update(dt)
	entity:SetPosition(5,5,5)
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
