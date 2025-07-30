local inspect = require("inspect")
local s
local c
local nux = nux

local yaw = 0
local pitch = 0
local monolith_canvas
local monolith_texture
local cube
local gui_canvas
local gui_texture
local arena

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

local api

local function generate_cart()
    assert(nux.io.cart_begin("cart.bin", 4))
    assert(nux.io.write_cart_file("cart.lua"))
    assert(nux.io.write_cart_file("inspect.lua"))
    assert(nux.io.write_cart_file("main.lua"))
    assert(nux.io.write_cart_file("assets/industrial.glb"))
    assert(nux.io.cart_end())
end

function nux.conf(conf)
    print("hello")
end

function nux.init()
    -- generate_cart()
    -- do return end
    arena = nux.arena.new(1 << 24)

    local mesh_cube = nux.mesh.new_cube(arena, 1, 1, 1)
    s = nux.scene.new(arena)

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
    end

    s = nux.scene.load_gltf(arena, "assets/industrial.glb")

    c = nux.node.new(s)
    nux.transform.add(c)
    nux.transform.set_translation(c, 0, 1, 3)
    nux.camera.add(c)
    nux.camera.set_fov(c, 70)

    -- Create canvas
    gui_canvas = nux.canvas.new(arena)
    gui_texture = nux.texture.new(arena, nux.TEXTURE_RENDER_TARGET, nux.CANVAS_WIDTH, nux.CANVAS_HEIGHT)

    -- Create the API monolith
    monolith_canvas = nux.canvas.new(arena)
    local x, y = 350, 1500
    monolith_texture = nux.texture.new(arena, nux.TEXTURE_RENDER_TARGET, x, y)
    cube = nux.node.new(s)
    nux.transform.add(cube)
    nux.transform.set_translation(cube, 10, 0, 0)
    nux.transform.set_scale(cube, x / 50, y / 50, 1)
    nux.staticmesh.add(cube)
    nux.staticmesh.set_mesh(cube, mesh_cube)
    nux.staticmesh.set_texture(cube, monolith_texture)

    api = inspect(nux)
end

function nux.tick()
    controller(c)
    -- nux.transform.rotate_y(cube, nux.dt() / 50)
    nux.scene.render(s, c)
    nux.canvas.clear(monolith_canvas)
    nux.canvas.text(monolith_canvas, 10, 10, string.format("time:%.2fs", nux.time()))
    nux.canvas.text(monolith_canvas, 10, 20, api)
    nux.canvas.render(monolith_canvas, monolith_texture)

    local x, y, z = nux.transform.get_translation(c)
    nux.canvas.clear(gui_canvas)
    nux.canvas.text(gui_canvas, 10, 10, string.format("time:%.2fs", nux.time()))
    nux.canvas.text(gui_canvas, 10, 20, string.format("x:%.2f", x))
    nux.canvas.text(gui_canvas, 10, 30, string.format("y:%.2f", y))
    nux.canvas.text(gui_canvas, 10, 40, string.format("z:%.2f", z))
    nux.canvas.render(gui_canvas, gui_texture)
    nux.texture.blit(gui_texture)

    -- print("test")
end
