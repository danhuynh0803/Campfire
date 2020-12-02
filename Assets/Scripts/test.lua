function OnTriggerEnter(other)
    other.Rigidbody.AddVelocity(0, 20, 0)
    Entity.Instantiate("cube.prefab", 0, 10, 0)
end