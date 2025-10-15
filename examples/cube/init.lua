function M:on_load()
    physics.set_ground_height(-10)
    -- create screen canvas
    self.canvas = canvas.new(arena.core(), 150, 100)
    local vp = viewport.new(arena.core(), graphics.screen())
    viewport.set_texture(vp, canvas.get_texture(self.canvas))
    -- create camera
    local e = node.create(node.root())
    self.camera = e
    camera.add(e)
    camera.set_fov(e, 50)
    transform.add(e)
    transform.set_translation(e, vmath.vec3(2))
    vp = viewport.new(arena.core(), graphics.screen())
    viewport.set_camera(vp, self.camera)
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
    transform.set_translation(self.camera, { 2, 1.2, 2 })
    transform.look_at(self.camera, { 0, 0, 0 })
    transform.rotate_y(self.cube, time.delta() * 0.5)
    transform.set_translation(self.cube, { 0, math.abs(math.sin(time.elapsed()) * 0.2), 0 })
end
