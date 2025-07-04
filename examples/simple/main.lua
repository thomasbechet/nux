-- local inspect = require("inspect")
local s
local c
local r
local nux = nux

local function controller(e)
    local x, y, z = nux.transform.get_translation(e)
    local speed = 10
    x = x + nux.input.axis(0, nux.AXIS_LEFTX) * nux.dt() * speed
    z = z - nux.input.axis(0, nux.AXIS_LEFTY) * nux.dt() * speed
    if nux.button.pressed(0, nux.BUTTON_Y) then
        y = y + nux.dt() * speed
    elseif nux.button.pressed(0, nux.BUTTON_X) then
        y = y - nux.dt() * speed
    end
    nux.transform.set_translation(e, x, y, z)
    local fx, _, fz = nux.transform.forward(e)
    nux.transform.look_at(c, x + fx, y - 0.5, z + fz)
    nux.transform.look_at(c, x + fx, y, z + fz)
end

function nux.init()
    -- s = nux.scene.load_gltf("../basic/assets/ariane6.glb")
    local mesh_cube = nux.mesh.gen_cube(1, 1, 1)
    s = nux.scene.new()

    local template = {
        staticmesh = { mesh = mesh_cube }
    }
    local template2 = {
        {
            transform = { translation = { 1, 0, 0 } },
            template = template
        },
        {
            transform = { translation = { -1, 0, 0 } },
            template = template
        },
        {
            transform = { translation = { 0, 1, 0 } },
            template = template
        }
    }
    local template3 = {
        {
            transform = { translation = { 1, 0, 0 } },
            template = template2,
        },
        {
            transform = { translation = { -1.5, 0, 0 } },
            template = template2,
        }
    }
    for i = 0, 5 do
        nux.node.instantiate(s, {
            template = template3,
            transform = { translation = { i * 10, 0, 0 } }
        }
        , nil)
    end

    c = nux.node.new(s)
    nux.transform.add(c)
    nux.transform.set_translation(c, 0, 1, 3)
    nux.camera.add(c)
    nux.camera.set_fov(c, 80)
end

function nux.tick()
    controller(c)
    nux.scene.draw(s, c)
    -- nux.transform.rotate_y(r, nux.dt() * 10)
end
