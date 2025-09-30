function M:on_load()
    self.speed = 10
    self.fast_speed = 20
    self.fov = 90
    self.pitch = 0
    self.yaw = 0
    self.entity = ecs.create(ecs.root())
    camera.add(self.entity)
    transform.add(self.entity)
    transform.set_translation(self.entity, { 13, 15, 10 })
end

function M:on_event(e)

end

function M:on_update()
    local speed = self.speed
    local fov = self.fov
    local e = self.entity

    if button.pressed(0, button.LB) then
        speed = self.fast_speed
    end
    camera.set_fov(e, fov)

    local mx = axis.value(0, axis.LEFTX)
    local mz = axis.value(0, axis.LEFTY)
    local my = 0
    if button.pressed(0, button.A) then
        my = 1
    elseif button.pressed(0, button.B) then
        my = -1
    end
    local rx = axis.value(0, axis.RIGHTX)
    local ry = axis.value(0, axis.RIGHTY)

    -- Translation
    local forward = transform.forward(e)
    local left = transform.left(e)
    local dt = time.delta()
    -- Forward
    local dir = forward * mz * dt * speed
    -- Left
    dir = dir - left * mx * dt * speed
    -- Up
    dir.y = dir.y + my * dt * speed
    local position = transform.get_translation(e)
    transform.set_translation(e, position + dir)

    -- Rotation
    if rx ~= 0 then
        self.yaw = self.yaw + rx * time.delta() * 100
    end
    if ry ~= 0 then
        self.pitch = self.pitch - ry * time.delta() * 100
    end
    self.pitch = math.clamp(self.pitch, -90, 90)
    transform.set_rotation_euler(e, vmath.vec3(-math.rad(self.pitch), -math.rad(self.yaw), 0))
    camera.set_far(e, 1000)
    camera.set_near(e, 0.1)
end
