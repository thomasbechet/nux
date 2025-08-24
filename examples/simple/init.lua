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
    config.hotreload = true
    print(inspect(config.graphics))
end

function nux.init()
    ARENA = nux.arena.core()
    PITCH = 0
    YAW = 0

    local mesh_cube = nux.mesh.new_cube(ARENA, 1, 1, 1)
    MESH_CUBE = mesh_cube

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

    local camera = nux.ecs.add()
    nux.transform.add(camera)
    nux.transform.set_translation(camera, 13, 15, 10)
    nux.camera.add(camera)
    nux.camera.set_fov(camera, 70)
    CAMERA = camera

    -- Create canvas
    GUI_CANVAS = nux.canvas.new(ARENA, nux.CANVAS_WIDTH, nux.CANVAS_HEIGHT, 4096)
    local e = nux.ecs.add()
    nux.canvaslayer.add(e)
    nux.canvaslayer.set_canvas(e, GUI_CANVAS)

    -- Create the API monolith
    local x, y = 350, 1600
    MONOLITH_CANVAS = nux.canvas.new(ARENA, x, y, 1000)
    CUBE = nux.ecs.add()
    nux.transform.add(CUBE)
    nux.transform.set_translation(CUBE, 10, 0, 0)
    nux.transform.set_scale(CUBE, x / 50, y / 50, 1)
    nux.staticmesh.add(CUBE)
    nux.staticmesh.set_mesh(CUBE, mesh_cube)
    nux.staticmesh.set_texture(CUBE, nux.canvas.get_texture(MONOLITH_CANVAS))
    nux.collider.add_aabb(CUBE, 0, 0, 0, x / 50, y / 50, 1)

    API = inspect(nux)
end

function nux.tick()
    controller(CAMERA)
    nux.transform.rotate_y(ROTATING, nux.time.delta() * math.sin(nux.time.elapsed()))
    nux.transform.set_scale(ROTATING, 1, 5, 10)

    local canvas = MONOLITH_CANVAS
    nux.canvas.text(canvas, 10, 10, string.format("time:%.2fs", nux.time.elapsed()))
    nux.canvas.text(canvas, 10, 20, API)
    nux.canvas.text(canvas, 50, 50, "hello Julia")

    local x, y, z = nux.transform.get_translation(CAMERA)
    local canvas = GUI_CANVAS
    nux.canvas.text(canvas, 10, 10, nux.time.date())
    nux.canvas.text(canvas, 10, 20, string.format("time:%.2fs", nux.time.elapsed()))
    nux.canvas.text(canvas, 10, 30, string.format("x:%.2f", x))
    nux.canvas.text(canvas, 10, 40, string.format("y:%.2f", y))
    nux.canvas.text(canvas, 10, 50, string.format("z:%.2f", z))
    nux.canvas.text(canvas, math.floor(nux.cursor.x(0)), math.floor(nux.cursor.y(0)), "X")

    local fx, fy, fz = nux.transform.forward(CAMERA)
    if nux.button.just_pressed(0, nux.BUTTON_RB) then
        local hit = nux.physics.query(x, y, z, fx, fy, fz)
        if hit then
            print("hit " .. hit)
        else
            local e = nux.ecs.add()
            nux.transform.add(e)
            nux.transform.set_translation(e, x, y, z)
            nux.rigidbody.add(e)
            local force = 10
            nux.rigidbody.set_velocity(e, fx * force, fy * force, fz * force)
            -- nux.staticmesh.add(e)
            -- nux.staticmesh.set_mesh(e, MESH_CUBE)
        end
    end
end
