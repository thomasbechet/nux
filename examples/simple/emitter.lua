function M:on_load()
    self.arena = resource.find("core_arena")
    self.time = 0
    self.pos = vmath.vec3(0, 10, 0)
    self.mesh = mesh.new_cube(self.arena, 1, 1, 1)
    self.force = 15
    self.interval = 0.5
end

function M:on_update()
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
        print(v)
        v = vmath.mul(v, 2)
        v = vmath.sub(v, 1)
        rigidbody.add(e)
        rigidbody.set_velocity(e, v * self.force)
        staticmesh.add(e)
        staticmesh.set_mesh(e, self.mesh)
        self.time = 0
    end
end
