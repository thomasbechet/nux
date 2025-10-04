local inspect = require("libs/inspect")
require("emitter")
local ticker = require("ticker")

function M:on_reload()
end

local function draw_hierarchy(nid, parent_position)
    local pos = transform.get_translation(nid)
    graphics.draw_line(pos, parent_position, 0xFFFFFF)
    local child = node.child(nid)
    while child do
        draw_hierarchy(child, pos)
        child = node.sibling(child)
    end
end

local function add_colliders(root)
    if staticmesh.has(root) then
        local m = staticmesh.get_mesh(root)
        local min = mesh.bounds_min(m)
        local max = mesh.bounds_max(m)
        collider.add_aabb(root, min, max)
    end
    local child = node.child(root)
    while child do
        add_colliders(child)
        child = node.sibling(child)
    end
end

function M:on_load()
    self.arena = arena.core()
    event.subscribe(self.rid, ticker.event)

    self.mesh_cube = mesh.new_cube(self.arena, 1, 1, 1)
    self.active_texture = nil
    self.active_mesh = nil

    self.scene = scene.load_gltf(self.arena, "assets/industrial.glb")
    scene.set_active(self.scene)
    local nid = node.instantiate(self.scene, node.root())
    transform.add(nid)
    transform.set_rotation_euler(nid, { 0, 90, 0 })
    transform.set_scale(nid, vmath.vec3(10))
    -- add all colliders before camera creation
    add_colliders(node.root())

    self.camera = require("camera")

    -- Create canvas
    self.gui_canvas = canvas.new(self.arena, canvas.WIDTH, canvas.HEIGHT)
    canvas.set_layer(self.gui_canvas, 1)

    -- Create the API monolith
    local x, y = 350, 2000
    self.monolith_canvas = canvas.new(self.arena, x, y)
    self.cube = node.create(node.root())
    transform.add(self.cube)
    transform.set_translation(self.cube, { 10, 0, 0 })
    transform.set_scale(self.cube, { x / 50, y / 50, 1 })
    staticmesh.add(self.cube)
    staticmesh.set_mesh(self.cube, self.mesh_cube)
    staticmesh.set_texture(self.cube, canvas.get_texture(self.monolith_canvas))
    collider.add_aabb(self.cube, vmath.vec3(0), { x / 50, y / 50, 1 })

    self.api = inspect(event)
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
    draw_hierarchy(node.root(), vmath.vec3(0))

    local p = vmath.vec3(0, 0, -10)
    graphics.draw_dir(p, vmath.vec3(1, 0, 0), 1, 0x0)
    graphics.draw_dir(p, vmath.vec3(0, 1, 0), 1, 0x0)
    graphics.draw_dir(p, vmath.vec3(0, 0, 1), 1, 0x0)

    local c = self.monolith_canvas
    canvas.set_clear_color(c, 0x99ccff)
    canvas.text(c, 10, 10, string.format("time:%.2fs", time.elapsed()))
    canvas.text(c, 10, 20, self.api)

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
        string.format("mem:%s", memhu(arena.memory_usage(arena.frame()))))
    canvas.text(c, math.floor(cursor.x(0)), math.floor(cursor.y(0)), "X")
    canvas.text(c, 10, 80, string.format("nodes:%d", scene.count()))

    local forward = transform.forward(self.camera.entity)
    if button.just_pressed(0, button.RB) then
        local hit = physics.raycast(position, forward)
        if hit and vmath.dist(hit.position, transform.get_translation(self.camera.entity)) < 10 then
            if staticmesh.has(hit.entity) then
                self.active_mesh = staticmesh.get_mesh(hit.entity)
                self.active_texture = staticmesh.get_texture(hit.entity)
                print("hit " .. hit.entity .. " mesh " .. self.active_mesh .. " texture " .. self.active_texture)
            end
        else
            if self.active_mesh and self.active_texture then
                local e = node.create(node.root())
                transform.add(e)
                transform.set_translation(e, position)
                local min = mesh.bounds_min(self.active_mesh)
                local max = mesh.bounds_max(self.active_mesh)
                collider.add_aabb(e, min, max)
                local force = 15
                rigidbody.add(e)
                rigidbody.set_velocity(e, forward * force)
                local child = node.create(e)
                transform.add(child)
                staticmesh.add(child)
                transform.set_translation(child, -min)
                staticmesh.set_mesh(child, self.active_mesh)
                staticmesh.set_texture(child, self.active_texture)
            end
        end
    end
end
