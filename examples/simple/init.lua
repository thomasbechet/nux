local inspect = require("inspect")
local nux = nux

local function controller(e)
    local speed = 20


    local mx = nux.axis(0, nux.AXIS_LEFTX)
    local mz = nux.axis(0, nux.AXIS_LEFTY)
    local my = 0
    if nux.button.pressed(0, nux.BUTTON_Y) then
        my = 1
    elseif nux.button.pressed(0, nux.BUTTON_X) then
        my = -1
    end
    local rx = nux.axis(0, nux.AXIS_RIGHTX)
    local ry = nux.axis(0, nux.AXIS_RIGHTY)

    -- Translation
    local fx, fy, fz = nux.transform.forward(e)
    local lx, ly, lz = nux.transform.left(e)
    local dx = 0
    local dy = 0
    local dz = 0
    local dt = nux.time.delta()
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
        YAW = YAW + rx * nux.time.delta() * 100
    end
    if ry ~= 0 then
        PITCH = PITCH - ry * nux.time.delta() * 100
    end
    PITCH = math.clamp(PITCH, -90, 90)
    nux.transform.set_rotation_euler(e, -math.rad(PITCH), -math.rad(YAW), 0)
    nux.camera.set_fov(e, 60)
    nux.camera.set_far(e, 1000)
    nux.camera.set_near(e, 1)
end

function nux.conf(config)
    print(inspect(config))
    config.hotreload = true
    config.arena.main_capacity = (1 << 27)
end

function nux.init()
    ARENA = nux.arena.main()
    PITCH = 0
    YAW = 0

    local mesh_cube = nux.mesh.new_cube(ARENA, 1, 1, 1)

    ECS = nux.ecs.load_gltf(ARENA, "assets/industrial.glb")
    nux.ecs.set_active(ECS)

    local template = {
        staticmesh = { mesh = mesh_cube }
    }
    local template2 = {
        {
            transform = { translation = { 2, 0, 0 } },
            template = template
        },
        {
            transform = { translation = { -2, 0, 0 } },
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
            transform = { translation = { -2, 0, 0 } },
            template = template2,
        }
    }
    for i = 0, 100 do
        local x = i // 10
        local y = i % 10
        local n = nux.ecs.instantiate({
            template = template3,
            transform = { translation = { x * 8, 0.1, y * 5 + 0.1 } }
        }
        , nil)
        if i == 50 then
            ROTATING = n
        end
    end

    C = nux.ecs.add()
    nux.transform.add(C)
    nux.transform.set_translation(C, 0, 1, 3)
    nux.camera.add(C)
    nux.camera.set_fov(C, 70)

    -- Create canvas
    GUI_CANVAS = nux.canvas.new(ARENA)
    GUI_TEXTURE = nux.texture.new(ARENA, nux.TEXTURE_RENDER_TARGET, nux.CANVAS_WIDTH, nux.CANVAS_HEIGHT)

    -- Create the API monolith
    MONOLITH_CANVAS = nux.canvas.new(ARENA)
    local x, y = 350, 1600
    MONOLITH_TEXTURE = nux.texture.new(ARENA, nux.TEXTURE_RENDER_TARGET, x, y)
    CUBE = nux.ecs.add()
    nux.transform.add(CUBE)
    nux.transform.set_translation(CUBE, 10, 0, 0)
    nux.transform.set_scale(CUBE, x / 50, y / 50, 1)
    nux.staticmesh.add(CUBE)
    nux.staticmesh.set_mesh(CUBE, mesh_cube)
    nux.staticmesh.set_texture(CUBE, MONOLITH_TEXTURE)

    API = inspect(nux)
end

function nux.tick()
    controller(C)
    nux.transform.rotate_y(ROTATING, nux.time.delta() * math.sin(nux.time.elapsed()))
    nux.transform.set_scale(ROTATING, 1, 5, 1)
    nux.ecs.render(C)
    nux.canvas.clear(MONOLITH_CANVAS)
    nux.canvas.text(MONOLITH_CANVAS, 10, 10, string.format("time:%.2fs", nux.time.elapsed()))
    nux.canvas.text(MONOLITH_CANVAS, 10, 20, API)
    nux.canvas.render(MONOLITH_CANVAS, MONOLITH_TEXTURE)

    local x, y, z = nux.transform.get_translation(C)
    nux.canvas.clear(GUI_CANVAS)
    nux.canvas.text(GUI_CANVAS, 10, 10, nux.time.date())
    nux.canvas.text(GUI_CANVAS, 10, 20, string.format("time:%.2fs", nux.time.elapsed()))
    nux.canvas.text(GUI_CANVAS, 10, 30, string.format("x:%.2f", x))
    nux.canvas.text(GUI_CANVAS, 10, 40, string.format("y:%.2f", y))
    nux.canvas.text(GUI_CANVAS, 10, 50, string.format("z:%.2f", z))
    nux.canvas.text(GUI_CANVAS, math.floor(nux.cursor.x(0)), math.floor(nux.cursor.y(0)), "X")

    nux.canvas.render(GUI_CANVAS, GUI_TEXTURE)
    nux.texture.blit(GUI_TEXTURE)
end
