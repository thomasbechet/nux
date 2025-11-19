function M:on_load()
    self.speed = 10
    self.fast_speed = 50
    self.slow_spped = 2
    self.fov = 90
    self.pitch = 0
    self.yaw = 0
    self.cam = node.create(node.root())
    self.velocity = vmath.vec3(0)
    self.max_speed = 10
    self.acc = 10
    node.add(self.cam, component.CAMERA)
    node.add(self.cam, component.TRANSFORM)
    camera.set_render_mask(self.cam, 1)
    camera.set_layer(self.cam, 1)
    camera.set_target(self.cam, texture.screen())
    transform.set_translation(self.cam, { 13, 15, 10 })
    self.cam_overlay = node.create(self.cam)
    node.add(self.cam_overlay, component.CAMERA)
    node.add(self.cam_overlay, component.TRANSFORM)
    -- camera.set_render_mask(self.cam_overlay, 2)
    -- camera.set_layer(self.cam_overlay, 2)
    -- camera.set_clear_depth(self.cam_overlay, false)
    -- camera.set_clear_color(self.cam_overlay, false)
    -- camera.set_target(self.cam_overlay, texture.screen())
    -- camera.set_viewport(self.cam_overlay, { 0, 0, 640, 340 })
end

function M:on_update()
    local speed = self.speed
    local e = self.cam

    if input.pressed(0, "sprint") then
        speed = self.fast_speed
    elseif input.pressed(0, "slow") then
        speed = self.slow_spped
    end

    local mx = input.value(0, "right") - input.value(0, "left")
    local mz = input.value(0, "forward") - input.value(0, "backward")
    local my = input.value(0, "up") - input.value(0, "down")
    local rx = input.value(0, "view_right") - input.value(0, "view_left")
    local ry = input.value(0, "view_up") - input.value(0, "view_down")

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

    -- Update velocity
    self.velocity = vmath.add(self.velocity, dir * time.delta() * self.acc)
    if vmath.length(self.velocity) > self.max_speed then
        self.velocity = vmath.normalize(self.velocity) * self.max_speed
    end
    self.velocity = vmath.mul(self.velocity, 0.90)

    -- Update position
    local position = transform.translation(e)
    transform.set_translation(e, position + self.velocity)

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
    camera.set_far(self.cam_overlay, 1000)
    camera.set_near(self.cam_overlay, 0.1)

    -- Update fov
    self.fov = self.fov + input.value(0, "zoom") - input.value(0, "dezoom")
    camera.set_fov(e, self.fov)
    camera.set_fov(self.cam_overlay, self.fov)
end
