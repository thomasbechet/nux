local LAYER = 0x2

function M:on_load()
    self.node = node.create(node.root())
    self.target = nil

    node.add(self.node, component.COLLIDER)
    node.add(self.node, component.TRANSFORM)
    collider.set_sphere(self.node, 1)
end

function M:update(pos, ray, btn)
    local hit = physics.raycast(pos, ray)
    if hit then
        local id = hit.node
        local just_pressed = button.just_pressed(0, btn)
        if just_pressed then
            if node.has(id, component.COLLIDER) and id ~= self.target then
                if self.target then
                    staticmesh.set_draw_bounds(self.target, false)
                end
                self.target = id
                staticmesh.set_draw_bounds(self.target, true)
            end
        end
    end
end

function M:on_update()
    -- cursor input
    -- draw gizmo
    if self.target then
        graphics.begin_state()
        graphics.set_layer(LAYER)
        graphics.set_transform(transform.get_matrix(self.target))
        local p = vmath.vec3(0)
        graphics.set_color(color.RED)
        graphics.draw_dir(p, vmath.vec3(1, 0, 0), 1)
        graphics.set_color(color.GREEN)
        graphics.draw_dir(p, vmath.vec3(0, 1, 0), 1)
        graphics.set_color(color.BLUE)
        graphics.draw_dir(p, vmath.vec3(0, 0, 1), 1)
        graphics.end_state()
    end
end
