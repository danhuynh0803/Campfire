function Start()
	print("Start")
end

function Update(dt)
	tf = Transform.New()
	print(tf.x)
	
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
