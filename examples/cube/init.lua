function M:on_load()
    physics.set_ground_height(-10)
    -- create screen canvas
    self.canvas = canvas.new(arena.core(), canvas.WIDTH, canvas.HEIGHT)
    canvas.set_layer(self.canvas, 1)
    -- create camera
    local e = node.create(node.root())
    self.camera = e
    camera.add(e)
    camera.set_fov(e, 100)
    transform.add(e)
    transform.set_translation(e, vmath.vec3(2))
    -- create cube
    local cube = mesh.new_cube(arena.core(), 1, 1, 1)
    mesh.set_origin(cube, vmath.vec3(0.5))
    local e = node.create(node.root())
    self.cube = e
    staticmesh.add(e)
    staticmesh.set_mesh(e, cube)
    transform.add(e)
end

function M:on_update()
    transform.look_at(self.camera, { 0, 0, 0 })
    transform.rotate_y(self.cube, time.delta() * 0.5)
    print(transform.get_translation(self.cube))
end
