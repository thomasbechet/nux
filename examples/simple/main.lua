local inspect = require("inspect")
local s
local c

print(inspect(nux))
local SCENE = require("scene")

function controller(e)
    local x, y, z = nux.transform.get_translation(e)
    x = x + nux.input.axis(0, nux.AXIS_LEFTX) * nux.dt() * 5
    z = z - nux.input.axis(0, nux.AXIS_LEFTY) * nux.dt() * 5
    if nux.button.pressed(0, nux.BUTTON_Y) then
        y = y + nux.dt() * 5
    elseif nux.button.pressed(0, nux.BUTTON_X) then
        y = y - nux.dt() * 5
    end
    nux.transform.set_translation(e, x, y, z)
    local fx, fy, fz = nux.transform.forward(e)
    nux.transform.look_at(c, x + fx, y - 0.5, z + fz)
end

function create_cube(scene, mesh, x, y, z)
    local n = nux.node.new(scene)
    print(mesh)
    nux.staticmesh.add(n)
    nux.staticmesh.set_mesh(n, mesh)
    nux.transform.add(n)
    nux.transform.set_translation(n, x, y, z)
    return n
end

function nux.init()
    s = nux.scene.new()

    nux.scene.parse(SCENE)

    local mesh_ariane = nux.mesh.load("../basic/assets/ariane6.glb")
    local mesh_cube = nux.mesh.gen_cube(1, 1, 1)

    create_cube(s, mesh_ariane, 0, 0, 0)

    c = nux.node.new(s)
    nux.transform.add(c)
    nux.transform.set_translation(c, 0, 1, 3)
    nux.camera.add(c)
    nux.camera.set_fov(c, 60)

    -- load_scene(SCENE)
end

function nux.tick()
    controller(c)
    nux.scene.draw(s, c)
end
