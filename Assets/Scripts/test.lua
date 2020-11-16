function Start()
	print("Start")
end

function Update(dt)
	tf = Transform.New()
	--print(tf.x,tf.y,tf.z)
	x = Tag.PushAOne()
	x = x + 1
	print(x)
	Transform.SetPosition(tf,1,1,1)
	--tf.SetPosition(1,1,1) //need to fix the access somehow
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
