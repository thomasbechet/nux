local inspect = require("inspect")
local nux = nux

Entity = {}
Entity.__index = Entity

function Entity.create()
    local entity = setmetatable({}, Entity)
    entity.id = nux.ecs.create()
    return entity
end

function Entity:delete()
    nux.ecs.delete(self.id)
end

Camera = {}
Camera.__index = Camera

function Camera.add(e)
    nux.camera.add(e.id)
    local camera = setmetatable({ id = e.id }, Camera)
    e.camera = camera
    return camera
end

function Camera:set_fov(fov)
    nux.camera.set_fov(self.id, fov)
end

Transform = {}
Transform.__index = Transform

function Transform.add(e)
    nux.transform.add(e.id)
    local transform = setmetatable({ id = e.id }, Transform)
    e.transform = transform
    return transform
end

function Transform:set_translation(x, y, z)
    nux.transform.set_translation(self.id, x, y, z)
end

function Transform:get_translation()
    return nux.transform.get_translation(self.id)
end

function Transform:forward()
    return nux.transform.forward(self.id)
end

function Transform:backward()
    return nux.transform.backward(self.id)
end

function Transform:set_scale(x, y, z)
    nux.transform.set_scale(self.id, x, y, z)
end

function Transform:set_parent(parent)
    nux.transform.set_parent(self.id, parent.id)
end

function Transform:set_rotation_euler(pitch, yaw, roll)
    nux.transform.set_rotation_euler(self.id, pitch, yaw, roll)
end

Staticmesh = {}
Staticmesh.__index = Staticmesh

function Staticmesh.add(e)
    nux.staticmesh.add(e.id)
    local staticmesh = setmetatable({ id = e.id }, Staticmesh)
    e.staticmesh = staticmesh
    return staticmesh
end

function Staticmesh:set_mesh(mesh)
    nux.staticmesh.set_mesh(self.id, mesh)
end

function Staticmesh:set_texture(tex)
    nux.staticmesh.set_texture(self.id, tex)
end

Canvas = {}
Canvas.__index = Canvas

function Canvas:new(arena, width, height, capa)
    local id = nux.canvas.new(arena, width, height, capa)
    return setmetatable({ id = id }, self)
end

function Canvas:text(x, y, text)
    nux.canvas.text(self.id, x, y, text)
end

CanvasLayer = {}
CanvasLayer.__index = CanvasLayer

function CanvasLayer.add(e)
    nux.canvaslayer.add(e.id)
    local canvaslayer = setmetatable({ id = e.id }, CanvasLayer)
    e.canvaslayer = canvaslayer
    return canvaslayer
end

function CanvasLayer:set_canvas(canvas)
    nux.canvaslayer.set_canvas(self.id, canvas)
end

Collider = {}
Collider.__index = Collider

function Collider.add_aabb(e, minx, miny, minz, maxx, maxy, maxz)
    nux.collider.add_aabb(e.id, minx, miny, minz, maxx, maxy, maxz)
    local collider = setmetatable({ id = e.id }, Collider)
    e.collider = collider
    return collider
end

RigidBody = {}
RigidBody.__index = RigidBody

function RigidBody.add(e)
    nux.rigidbody.add(e.id)
    local rigidbody = setmetatable({ id = e.id }, RigidBody)
    e.rigidbody = rigidbody
    return rigidbody
end

function RigidBody:set_velocity(vx, vy, vz)
    nux.rigidbody.set_velocity(self.id, vx, vy, vz)
end

local function controller(e)
    local speed = 10
    local fast = speed * 2

    if nux.button.pressed(0, nux.BUTTON_LB) then
        speed = fast
    end

    local mx = nux.axis(0, nux.AXIS_LEFTX)
    local mz = nux.axis(0, nux.AXIS_LEFTY)
    local my = 0
    if nux.button.pressed(0, nux.BUTTON_A) then
        my = 1
    elseif nux.button.pressed(0, nux.BUTTON_B) then
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

    local cam = Entity.create()
    Camera.add(cam)
    Transform.add(cam)
    cam.camera:set_fov(70)
    cam.transform:set_translation(13, 15, 10)
    CAMERA = cam.id

    -- Create canvas
    GUI_CANVAS = nux.canvas.new(ARENA, nux.CANVAS_WIDTH, nux.CANVAS_HEIGHT, 4096)
    local e = nux.ecs.create()
    nux.canvaslayer.add(e)
    nux.canvaslayer.set_canvas(e, GUI_CANVAS)

    -- Create the API monolith
    local x, y = 350, 1600
    MONOLITH_CANVAS = nux.canvas.new(ARENA, x, y, 1000)
    CUBE = nux.ecs.create()
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
    canvas = GUI_CANVAS
    nux.canvas.text(canvas, 10, 10, nux.time.date())
    nux.canvas.text(canvas, 10, 20, string.format("time:%.2fs", nux.time.elapsed()))
    nux.canvas.text(canvas, 10, 30, string.format("x:%.2f", x))
    nux.canvas.text(canvas, 10, 40, string.format("y:%.2f", y))
    nux.canvas.text(canvas, 10, 50, string.format("z:%.2f", z))
    nux.canvas.text(canvas, math.floor(nux.cursor.x(0)), math.floor(nux.cursor.y(0)), "X")

    local fx, fy, fz = nux.transform.forward(CAMERA)
    if nux.button.just_pressed(0, nux.BUTTON_RB) then
        local hit = nux.physics.query(x, y, z, fx, fy, fz)
        if false then
            print("hit " .. hit)
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

            local e = Entity.create()
            Transform.add(e)
            e.transform:set_translation(x, y, z)
            local minx, miny, minz = nux.mesh.bounds_min(m)
            local maxx, maxy, maxz = nux.mesh.bounds_max(m)
            Collider.add_aabb(e, minx, miny, minz, maxx, maxy, maxz)
            local force = 15
            RigidBody.add(e)
            e.rigidbody:set_velocity(fx * force, fy * force, fz * force)


            -- add mesh
            local child = Entity.create()
            Transform.add(child)
            Staticmesh.add(child)
            child.transform:set_parent(e)
            child.transform:set_translation(-minx, -miny, -minz)
            child.staticmesh:set_mesh(m)
            child.staticmesh:set_texture(t)
        end
    end
end
