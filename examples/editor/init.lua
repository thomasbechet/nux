local function add_colliders(root)
    if node.has(root, component.STATICMESH) then
        local m = staticmesh.get_mesh(root)
        mesh.update_bounds(m)
        local min = mesh.bounds_min(m)
        local max = mesh.bounds_max(m)
        node.add(root, component.COLLIDER)
        collider.set_aabb(root, min, max)
    end
    local child = node.child(root)
    while child do
        add_colliders(child)
        child = node.sibling(child)
    end
end

local WIDTH = 640
local HEIGHT = 480

function M:on_load()
    self.arena = arena.core()

    self.mesh_cube = mesh.new_cube(self.arena, 1, 1, 1)
    self.active_texture = nil
    self.active_mesh = nil

    self.scene = scene.load_gltf(self.arena, "assets/industrial.glb")
    scene.set_active(self.scene)
    local nid = node.instantiate(self.scene, node.root())
    node.add(nid, component.TRANSFORM)
    transform.set_rotation_euler(nid, { 0, 90, 0 })
    transform.set_scale(nid, vmath.vec3(10))
    -- add all colliders before camera creation
    add_colliders(node.root())

    self.camera = require("camera")
    self.gizmos = require("gizmos")

    local vp = viewport.new(self.arena, texture.screen())
    viewport.set_camera(vp, self.camera.node)
    viewport.set_extent(vp, { 0, 0, 1, 1 })
    viewport.set_layer(vp, 0)
    viewport.set_mode(vp, viewport.STRETCH_KEEP_ASPECT)

    local vp = viewport.new(self.arena, texture.screen())
    viewport.set_camera(vp, self.camera.top_node)
    viewport.set_extent(vp, { 0, 0, 1, 1 })
    viewport.set_layer(vp, 1)
    viewport.set_clear_depth(vp, true)
    viewport.set_mode(vp, viewport.STRETCH_KEEP_ASPECT)

    self.gui_canvas = canvas.new(self.arena, WIDTH, HEIGHT)
    self.vp = viewport.new(self.arena, texture.screen())
    viewport.set_extent(self.vp, { 0, 0, 1, 1 })
    viewport.set_texture(self.vp, canvas.get_texture(self.gui_canvas))
    viewport.set_mode(self.vp, viewport.STRETCH_KEEP_ASPECT)
    viewport.set_layer(self.vp, 2)
end

-- local function get_screen()

function M:on_update()
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
    local gcur = cursor.get(0)
    local cur = viewport.to_local(self.vp, gcur)
    canvas.text(c, 10, 70, string.format("cursor: %.2f %.2f", cur.x, cur.y))

    canvas.rectangle(c, cur.x * WIDTH - 1, cur.y * HEIGHT - 1, 5, 5)

    camera.reset_aspect(self.camera.node, self.vp)
    camera.reset_aspect(self.camera.top_node, self.vp)

    local dir = camera.unproject(self.camera.node, gcur)
    self.gizmos:update(position, dir, button.RB)
end
