function M:on_load()
    self.cam = node.create(node.root())
    transform.add(self.cam)
    transform.set_translation(self.cam, vmath.vec3(1000))
    camera.add(self.cam)
    camera.set_ortho(self.cam, true)
    camera.set_ortho_size(self.cam, { 2, 2 })
    local vp = viewport.new(arena.core(), texture.screen())
    viewport.set_extent(vp, { 0.8, 0, 0.2, 0.2 })
    -- viewport.set_anchor(vp, anchor.TOP | anchor.RIGHT)
    viewport.set_camera(vp, self.cam)
    viewport.set_layer(vp, 5)
    self.target = nil
end

function M:on_update()
    if self.target then
        graphics.begin_state()
        camera.set_ortho(self.cam, true)
        -- Copy rotation
        transform.set_rotation(self.cam, transform.get_rotation(self.target))
        -- Draw rotation axis in front of camera
        local pos = transform.get_translation(self.cam)
        vmath.add(pos, transform.forward(self.cam) * 10, pos)
        graphics.set_color(color.RED)
        graphics.draw_dir(pos, vmath.vec3(1, 0, 0), 1)
        graphics.set_color(color.GREEN)
        graphics.draw_dir(pos, vmath.vec3(0, 1, 0), 1)
        graphics.set_color(color.BLUE)
        graphics.draw_dir(pos, vmath.vec3(0, 0, 1), 1)
        graphics.end_state()
    end
end
