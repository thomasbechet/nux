local inspect = require("libs/inspect")
require("emitter")

function M:on_reload()
end

function M:on_event(e)
end

function M:on_load()
    self.arena = resource.find("core_arena")

    local mesh_cube = mesh.new_cube(self.arena, 1, 1, 1)
    self.cube_mesh = mesh_cube

    self.ecs = ecs.load_gltf(self.arena, "assets/industrial.glb")
    ecs.set_active(self.ecs)
    self.camera = require("camera")

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
        local n = ecs.instantiate({
            template = template3,
            transform = { translation = { x * 8, 0.1, y * 5 + 0.1 } }
        }
        , nil)
        if i == 50 then
            self.rotating = n
        end
    end

    -- Create canvas
    self.gui_canvas = canvas.new(self.arena, canvas.WIDTH, canvas.HEIGHT)
    canvas.set_layer(self.gui_canvas, 1)

    -- Create the API monolith
    local x, y = 350, 2000
    self.monolith_canvas = canvas.new(self.arena, x, y)
    self.cube = ecs.create()
    transform.add(self.cube)
    transform.set_translation(self.cube, { 10, 0, 0 })
    transform.set_scale(self.cube, { x / 50, y / 50, 1 })
    staticmesh.add(self.cube)
    staticmesh.set_mesh(self.cube, mesh_cube)
    staticmesh.set_texture(self.cube, canvas.get_texture(self.monolith_canvas))
    collider.add_aabb(self.cube, vmath.vec3(0), { x / 50, y / 50, 1 })

    self.api = inspect(transform)
end

local function memhu(size)
    local i = 1
    while size > 1024 do
        i = i + 1
        size = size / 1024
    end
    local units = { "B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" }
    return string.format("%.02f%s", size, units[i])
end

function M:on_update()
    transform.rotate_y(self.rotating, time.delta() * math.sin(time.elapsed()))
    transform.set_scale(self.rotating, vmath.vec3(1, 5, 10))

    -- nux.graphics.draw_line(nux.vmath.vec3(0, 0, 0), nux.vmath.vec3(10, 10, 10), 0x0)
    local p = vmath.vec3(0, 0, -10)
    graphics.draw_dir(p, vmath.vec3(1, 0, 0), 1, 0x0)
    graphics.draw_dir(p, vmath.vec3(0, 1, 0), 1, 0x0)
    graphics.draw_dir(p, vmath.vec3(0, 0, 1), 1, 0x0)

    local c = self.monolith_canvas
    canvas.set_clear_color(c, 0x99ccff)
    canvas.text(c, 10, 10, string.format("time:%.2fs", time.elapsed()))
    canvas.text(c, 10, 20, self.api)
    canvas.text(c, 150, 50, "hello Julia")

    local position = transform.get_translation(self.camera.entity)
    c = self.gui_canvas
    canvas.text(c, 10, 10, time.date())
    canvas.text(c, 10, 20, string.format("time:%.2fs", time.elapsed()))
    canvas.text(c, 10, 30, string.format("x:%.2f", position.x))
    canvas.text(c, 10, 40, string.format("y:%.2f", position.y))
    canvas.text(c, 10, 50, string.format("z:%.2f", position.z))
    canvas.text(c, 10, 60,
        string.format("mem:%s bc:%d", memhu(arena.memory_usage(self.arena)), arena.block_count(self.arena)))
    canvas.text(c, 10, 70,
        string.format("mem:%s", memhu(arena.memory_usage(resource.find("frame_arena")))))
    canvas.text(c, math.floor(cursor.x(0)), math.floor(cursor.y(0)), "X")

    local forward = transform.forward(self.camera.entity)
    if button.just_pressed(0, button.RB) then
        local hit = physics.raycast(position, forward)
        if hit then
            local pos = hit.position
            print("hit at " .. tostring(pos))
            local e = ecs.create()
            transform.add(e)
            transform.set_translation(e, pos)
            staticmesh.add(e)
            staticmesh.set_mesh(e, self.cube_mesh)
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
            local m, t = table.unpack(r[(core.random() % #r) + 1])

            local e = ecs.create()
            transform.add(e)
            transform.set_translation(e, position)
            local min = mesh.bounds_min(m)
            local max = mesh.bounds_max(m)
            collider.add_aabb(e, min, max)
            local force = 15
            rigidbody.add(e)
            rigidbody.set_velocity(e, forward * force)

            -- add mesh
            local child = ecs.create()
            transform.add(child)
            staticmesh.add(child)
            transform.set_parent(child, e)
            transform.set_translation(child, -min)
            staticmesh.set_mesh(child, m)
            staticmesh.set_texture(child, t)
        end
    end
end
