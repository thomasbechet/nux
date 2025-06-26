local inspect = require("inspect")
local s
local c
local c0

function controller(e)
    local x, y, z = nux.transform.get_translation(e)
    x = x + nux.input.axis(0, nux.AXIS_LEFTX) * nux.dt() * 5
    z = z - nux.input.axis(0, nux.AXIS_LEFTY) * nux.dt() * 5
    nux.transform.set_translation(e, x, 3, z)
    local fx, fy, fz = nux.transform.forward(e)
    nux.transform.look_at(c, x + fx, y - 0.5, z + fz)
end

function create_cube(scene, mesh, x, y, z)
    local e = nux.entity.new(scene)
    nux.staticmesh.add(e)
    nux.staticmesh.set_mesh(e, mesh)
    nux.transform.add(e)
    nux.transform.set_translation(e, x, y, z)
    return e
end

function nux.init()
    s = nux.scene.new()

    local mesh = nux.mesh.gen_cube(1, 1, 1)
    for i = 0, 50 do
        create_cube(s, mesh, (i / 10) * 1.2, 0, (i % 10) * 1.2)
    end
    create_cube(s, mesh, 0.6, 0, 0)
    c0 = create_cube(s, mesh, 0, 1.1, 0)
    local c1 = create_cube(s, mesh, -1.6, 0, 0)
    nux.transform.set_scale(c1, 1.1, 1, 1.2)

    nux.transform.set_parent(c1, c0)

    c = nux.entity.new(s)
    nux.transform.add(c)
    nux.transform.set_translation(c, 0, 1, 3)
    nux.camera.add(c)
    nux.camera.set_fov(c, 60)
end

function nux.tick()
    local t = nux.time()
    nux.transform.rotate_y(c0, 0.5)
    controller(c)
    nux.scene.draw(s, c)
end
