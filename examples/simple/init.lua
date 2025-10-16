local inspect = require("libs/inspect")
require("emitter")
local ticker = require("ticker")
local gizmos

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

local WIDTH = 200
local HEIGHT = 100

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
    gizmos = require("gizmos")
    gizmos.target = self.camera.node

    local vp = viewport.new(self.arena, graphics.screen())
    viewport.set_camera(vp, self.camera.node)
    viewport.set_extent(vp, { 0, 0, 1, 1 })
    viewport.set_layer(vp, -1)

    -- local vp = viewport.new(self.arena, graphics.screen())
    -- viewport.set_camera(vp, self.camera.top_node)
    -- viewport.set_extent(vp, { 0, 0, 1, 1 })
    -- viewport.set_layer(vp, 2)
    -- viewport.set_clear_depth(vp, true)

    -- Create canvas
    self.gui_canvas = canvas.new(self.arena, WIDTH, HEIGHT)
    self.vp = viewport.new(self.arena, graphics.screen())
    viewport.set_extent(self.vp, { 0, 0, 0.3, 0.3 })
    viewport.set_texture(self.vp, canvas.get_texture(self.gui_canvas))
    viewport.set_anchor(self.vp, anchor.TOP | anchor.LEFT)
    viewport.set_mode(self.vp, viewport.STRETCH_KEEP_ASPECT)
    viewport.set_layer(self.vp, 3)

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
    staticmesh.set_render_layer(self.cube, 0x101)
    collider.add_aabb(self.cube, vmath.vec3(0), { x / 50, y / 50, 1 })

    self.api = inspect(event)
end

function M:on_update()
    -- draw_hierarchy(node.root(), vmath.vec3(0))

    local p = vmath.vec3(0, 0, -10)
    graphics.draw_dir(p, vmath.vec3(1, 0, 0), 2, color.RED)
    graphics.draw_dir(p, vmath.vec3(0, 1, 0), 2, color.GREEN)
    graphics.draw_dir(p, vmath.vec3(0, 0, 1), 2, color.BLUE)

    local c = self.monolith_canvas
    canvas.set_clear_color(c, color.BACKGROUND)
    canvas.text(c, 10, 10, string.format("time: %.2fs", time.elapsed()))
    canvas.text(c, 10, 20, self.api)

    local position = transform.get_translation(self.camera.node)
    c = self.gui_canvas
    canvas.text(c, 10, 10, time.date())
    canvas.text(c, 10, 20, string.format("time: %.2fs", time.elapsed()))
    canvas.text(c, 10, 30, string.format("xyz: %.2f %.2f %.2f", position.x, position.y, position.z))
    canvas.text(c, 10, 40,
        string.format("core mem: %s bc:%d",
            math.memhu(arena.memory_usage(arena.core())),
            arena.block_count(arena.core())))
    canvas.text(c, 10, 50,
        string.format("frame mem: %s bc:%d",
            math.memhu(arena.memory_usage(arena.frame())),
            arena.block_count(arena.frame())))
    canvas.text(c, 10, 60, string.format("nodes: %d", scene.count()))
    local cp = cursor.get(0)
    canvas.text(c, 10, 70, string.format("cursor: %.2f %.2f", cp.x, cp.y))

    local cp = cursor.get(0)
    canvas.rectangle(c, cp.x * WIDTH - 1, cp.y * HEIGHT - 1, 3, 3)
    canvas.rectangle(c, WIDTH / 2, HEIGHT / 2, 3, 3);

    camera.reset_aspect(self.camera.node, self.vp)

    local forward = transform.forward(self.camera.node)
    if button.just_pressed(0, button.RB) then
        local dir = camera.unproject(self.camera.node, cursor.get(0))
        local hit = physics.raycast(position, dir)
        -- local dist = vmath.dist(hit.position, transform.get_translation(self.camera.node))
        if hit then
            if staticmesh.has(hit.node) then
                self.active_mesh = staticmesh.get_mesh(hit.node)
                self.active_texture = staticmesh.get_texture(hit.node)
                print("hit " .. self.active_mesh)
                staticmesh.set_render_layer(hit.node, 1)
            end
        else
            if self.active_mesh and self.active_texture then
                -- rigidbody node
                local e = node.create(node.root())
                transform.add(e)
                transform.set_translation(e, position)
                local min = mesh.bounds_min(self.active_mesh)
                local max = mesh.bounds_max(self.active_mesh)
                collider.add_aabb(e, min, max)
                local force = 15
                rigidbody.add(e)
                rigidbody.set_velocity(e, forward * force)
                -- staticmesh node
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
