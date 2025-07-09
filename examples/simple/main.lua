local inspect = require("inspect")
local s
local c
local r
local nux = nux

local yaw = 0
local pitch = 0
local canvas
local surface
local cube

local function controller(e)
    local speed = 10

    local mx = nux.input.axis(0, nux.AXIS_LEFTX)
    local mz = nux.input.axis(0, nux.AXIS_LEFTY)
    local my = 0
    if nux.button.pressed(0, nux.BUTTON_Y) then
        my = 1
    elseif nux.button.pressed(0, nux.BUTTON_X) then
        my = -1
    end
    local rx = nux.input.axis(0, nux.AXIS_RIGHTX)
    local ry = nux.input.axis(0, nux.AXIS_RIGHTY)

    -- Translation
    local fx, fy, fz = nux.transform.forward(e)
    local lx, ly, lz = nux.transform.left(e)
    local dx = 0
    local dy = 0
    local dz = 0
    local dt = nux.dt()
    -- Forward
    dx = dx + fx * mz * dt * speed
    dy = dy + fy * mz * dt * speed
    dz = dz + fz * mz * dt * speed
    -- Left
    dx = dx - lx * mx * dt * speed
    dy = dy - ly * mx * dt * speed
    dz = dz - lz * mx * dt * speed
    -- Up
    dy = dy + my * dt * speed
    local x, y, z = nux.transform.get_translation(e)
    nux.transform.set_translation(e, x + dx, y + dy, z + dz)

    -- Rotation
    if rx ~= 0 then
        yaw = yaw + rx * nux.dt() * 100
    end
    if ry ~= 0 then
        pitch = pitch - ry * nux.dt() * 100
    end
    pitch = math.clamp(pitch, -90, 90)
    nux.transform.set_rotation_euler(e, -math.rad(pitch), -math.rad(yaw), 0)
end

function nux.init()
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
    for i = 0, 100 do
        local x = i // 10
        local y = i % 10
        local n = nux.node.instantiate(s, {
            template = template3,
            transform = { translation = { x * 5, 0, y * 5 } }
        }
        , nil)
        if i == 55 then
            r = n
        end
    end

    s = nux.scene.load_gltf("../basic/assets/industrial.glb")

    c = nux.node.new(s)
    nux.transform.add(c)
    nux.transform.set_translation(c, 0, 1, 3)
    nux.camera.add(c)
    nux.camera.set_fov(c, 80)

    canvas = nux.canvas.new()
    local x, y = 350, 1500
    surface = nux.texture.new(nux.TEXTURE_RENDER_TARGET, x, y)

    cube = nux.node.new(s)
    nux.transform.add(cube)
    nux.transform.set_translation(cube, 0, 5, 0)
    nux.transform.set_scale(cube, x / 100, y / 100, 1)
    nux.staticmesh.add(cube)
    nux.staticmesh.set_mesh(cube, mesh_cube)
    nux.staticmesh.set_texture(cube, surface)
end

function nux.tick()
    controller(c)
    nux.transform.rotate_y(r, nux.dt() * 10)
    nux.transform.rotate_y(cube, nux.dt() / 10)
    nux.scene.render(s, c)
    nux.canvas.clear(canvas)
    for i = 0, 100 do
        nux.canvas.text(canvas, 10, 10 * i, string.format("%f", math.random()))
    end
    -- nux.canvas.text(canvas, 10, 10, string.format("time:%.2fs", nux.time()))
    -- nux.canvas.text(canvas, 10, 20, inspect(nux))
    nux.canvas.render(canvas, surface)
    -- nux.texture.blit(surface)
end
