local inspect = require("inspect")
local nux = nux

local i = 0

local function controller(e)
    -- print(i)
    i = i + 1
    local speed = 10
    local fast = speed * 2

    if nux.button.pressed(0, nux.button.LB) then
        speed = fast
    end

    local mx = nux.axis.value(0, nux.axis.LEFTX)
    local mz = nux.axis.value(0, nux.axis.LEFTY)
    local my = 0
    if nux.button.pressed(0, nux.button.A) then
        my = 1
    elseif nux.button.pressed(0, nux.button.B) then
        my = -1
    end
    local rx = nux.axis.value(0, nux.axis.RIGHTX)
    local ry = nux.axis.value(0, nux.axis.RIGHTY)

    -- Translation
    local forward = nux.transform.forward(e)
    local left = nux.transform.left(e)
    local dt = nux.time.delta()
    -- Forward
    local dir = forward * mz * dt * speed
    -- Left
    dir = dir - left * mx * dt * speed
    -- Up
    dir.y = dir.y + my * dt * speed
    local position = nux.transform.get_translation(e)
    nux.transform.set_translation(e, position + dir)

    -- Rotation
    if rx ~= 0 then
        YAW = YAW + rx * nux.time.delta() * 100
    end
    if ry ~= 0 then
        PITCH = PITCH - ry * nux.time.delta() * 100
    end
    PITCH = math.clamp(PITCH, -90, 90)
    nux.transform.set_rotation_euler(e, nux.vmath.vec3(-math.rad(PITCH), -math.rad(YAW), 0))
    nux.camera.set_fov(e, 90)
    nux.camera.set_far(e, 1000)
    nux.camera.set_near(e, 0.1)
end

function nux.conf(config)
    config.hotreload = true
    -- config.log.level = 'debug'
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

    local cam = nux.ecs.create()
    nux.camera.add(cam)
    nux.transform.add(cam)
    nux.camera.set_fov(cam, 70)
    nux.transform.set_translation(cam, { 13, 15, 10 })
    CAMERA = cam

    -- Create canvas
    GUI_CANVAS = nux.canvas.new(ARENA, nux.canvas.WIDTH, nux.canvas.HEIGHT, 4096)
    nux.canvas.set_layer(GUI_CANVAS, 1)

    -- Create the API monolith
    local x, y = 350, 2000
    MONOLITH_CANVAS = nux.canvas.new(ARENA, x, y, 1000)
    CUBE = nux.ecs.create()
    nux.transform.add(CUBE)
    nux.transform.set_translation(CUBE, { 10, 0, 0 })
    nux.transform.set_scale(CUBE, { x / 50, y / 50, 1 })
    nux.staticmesh.add(CUBE)
    nux.staticmesh.set_mesh(CUBE, mesh_cube)
    nux.staticmesh.set_texture(CUBE, nux.canvas.get_texture(MONOLITH_CANVAS))
    nux.collider.add_aabb(CUBE, nux.vmath.vec3(0), { x / 50, y / 50, 1 })

    API = inspect(nux)
end

function nux.tick()
    controller(CAMERA)
    nux.transform.rotate_y(ROTATING, nux.time.delta() * math.sin(nux.time.elapsed()))
    nux.transform.set_scale(ROTATING, nux.vmath.vec3(1, 5, 10))

    local canvas = MONOLITH_CANVAS
    nux.canvas.set_clear_color(canvas, 0xFF0000)
    nux.canvas.text(canvas, 10, 10, string.format("time:%.2fs", nux.time.elapsed()))
    nux.canvas.text(canvas, 10, 20, API)
    nux.canvas.text(canvas, 150, 50, "hello Julia")

    local position = nux.transform.get_translation(CAMERA)
    canvas = GUI_CANVAS
    nux.canvas.text(canvas, 10, 10, nux.time.date())
    nux.canvas.text(canvas, 10, 20, string.format("time:%.2fs", nux.time.elapsed()))
    nux.canvas.text(canvas, 10, 30, string.format("x:%.2f", position.x))
    nux.canvas.text(canvas, 10, 40, string.format("y:%.2f", position.y))
    nux.canvas.text(canvas, 10, 50, string.format("z:%.2f", position.z))
    nux.canvas.text(canvas, math.floor(nux.cursor.x(0)), math.floor(nux.cursor.y(0)), "X")

    local forward = nux.transform.forward(CAMERA)
    if nux.button.just_pressed(0, nux.button.RB) then
        local hit = nux.physics.raycast(position, forward)
        if hit then
            local e = nux.ecs.create()
            nux.transform.add(e)
            nux.transform.set_translation(e, hit.position)
            nux.staticmesh.add(e)
            nux.staticmesh.set_mesh(e, MESH_CUBE)
        else
            local r = {
                { 0x100001C, 0x1000069 },
                { 0x1000021, 0x100006C },
                { 0x100001F, 0x100006B },
                { 0x1000051, 0x100007E },
                { 0x1000052, 0x100007E },
                { 0x1000053, 0x100007E },
                { 0x1000048, 0x100007C },
            }
            local m, t = table.unpack(r[(nux.random() % #r) + 1])

            local e = nux.ecs.create()
            nux.transform.add(e)
            nux.transform.set_translation(e, position)
            local min = nux.mesh.bounds_min(m)
            local max = nux.mesh.bounds_max(m)
            nux.collider.add_aabb(e, min, max)
            local force = 15
            nux.rigidbody.add(e)
            nux.rigidbody.set_velocity(e, forward * force)

            -- add mesh
            local child = nux.ecs.create()
            nux.transform.add(child)
            nux.staticmesh.add(child)
            nux.transform.set_parent(child, e)
            nux.transform.set_translation(child, -min)
            nux.staticmesh.set_mesh(child, m)
            nux.staticmesh.set_texture(child, t)
        end
    end
end
