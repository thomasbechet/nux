function M:on_load()
    -- create screen canvas
    self.canvas = canvas.new(arena.core(), canvas.WIDTH, canvas.HEIGHT)
    canvas.set_layer(self.canvas, 1)
    -- create camera
    local e = node.create(node.root())
    self.camera = e
    camera.add(e)
    camera.set_fov(e, 40)
    transform.add(e)
    transform.set_translation(e, vmath.vec3(2))
    -- create cube
    e = node.create(node.root())
    self.cube = e
    transform.add(e)
    transform.set_translation(e, vmath.vec3(-0.5))
    staticmesh.add(e)
    staticmesh.set_mesh(e, mesh.new_cube(arena.core(), 1, 1, 1))
end

function M:on_update()
    transform.look_at(self.camera, { 0, 0, 0 })
    transform.rotate_y(self.cube, time.delta() * 0.5)
end
