function M:on_load()
    self.arena = arena.core()
    self.time = 0
    self.pos = vmath.vec3(0, 10, 0)
    self.mesh = mesh.new_cube(self.arena, 1, 1, 1)
    self.force = 50
    self.interval = 0.5
    self.count = 0
end

function M:on_update()
    self.force = 10
    self.time = self.time + time.delta()
    if self.time > self.interval then
        local e = ecs.create()
        transform.add(e)
        transform.set_translation(e, self.pos)
        local min = mesh.bounds_min(self.mesh)
        local max = mesh.bounds_max(self.mesh)
        collider.add_aabb(e, min, max)
        local v = vmath.vec3(
            core.random01(),
            core.random01(),
            core.random01()
        )
        v = vmath.mul(v, 2)
        v = vmath.sub(v, 1)
        rigidbody.add(e)
        rigidbody.set_velocity(e, v * self.force)
        staticmesh.add(e)
        staticmesh.set_mesh(e, self.mesh)
        self.count = self.count + 1
        print("emit count : " .. self.count)
        self.time = 0
    end
end
