local function add_colliders(root)
    if node.has(root, component.STATICMESH) then
        local m = staticmesh.mesh(root)
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

    print(arena.core())
    print(arena.frame())
    local map = inputmap.new(self.arena)
    inputmap.bind_key(map, "up", key.X)
    inputmap.bind_key(map, "down", key.Z)
    inputmap.bind_key(map, "left", key.A)
    inputmap.bind_key(map, "right", key.D)
    inputmap.bind_key(map, "forward", key.W)
    inputmap.bind_key(map, "backward", key.S)
    inputmap.bind_key(map, "sprint", key.LEFT_SHIFT)
    inputmap.bind_key(map, "slow", key.LEFT_CONTROL)
    inputmap.bind_mouse_button(map, "click", mouse.BUTTON_LEFT)
    inputmap.bind_mouse_axis(map, "view_right", mouse.MOTION_RIGHT, 0.02)
    inputmap.bind_mouse_axis(map, "view_left", mouse.MOTION_LEFT, 0.02)
    inputmap.bind_mouse_axis(map, "view_up", mouse.MOTION_UP, 0.02)
    inputmap.bind_mouse_axis(map, "view_down", mouse.MOTION_DOWN, 0.02)
    inputmap.bind_mouse_axis(map, "zoom", mouse.SCROLL_UP, 1)
    inputmap.bind_mouse_axis(map, "dezoom", mouse.SCROLL_DOWN, 1)

    -- inputmap.bind_key(map, "view_right", key.L)
    -- inputmap.bind_key(map, "view_left", key.H)
    -- inputmap.bind_key(map, "view_up", key.K)
    -- inputmap.bind_key(map, "view_down", key.J)

    input.set_inputmap(0, map)

    self.mesh_cube = mesh.new_cube(self.arena, 1, 1, 1)

    self.scene = scene.load_gltf(self.arena, "assets/industrial.glb")
    scene.set_active(self.scene)
    local nid = node.instantiate(self.scene, node.root())
    node.add(nid, component.TRANSFORM)
    transform.set_translation(nid, { 0, 0, 0 })
    for i = 0, 2 do
        local child = node.instantiate(self.scene, node.root())
        node.add(child, component.TRANSFORM)
        transform.set_translation(child, { i * 100, 0, 0 })
    end

    -- add all colliders before camera creation
    add_colliders(node.root())

    self.camera = require("camera")
    self.gizmos = require("gizmos")

    local vp = viewport.new(self.arena, texture.screen())
    viewport.set_camera(vp, self.camera.node)
    -- viewport.set_extent(vp, { 0, 0, 1, 1 })
    viewport.set_layer(vp, 0)
    viewport.set_mode(vp, viewport.STRETCH_KEEP_ASPECT)
    viewport.set_auto_resize(vp, true)

    local vp = viewport.new(self.arena, texture.screen())
    viewport.set_camera(vp, self.camera.top_node)
    -- viewport.set_extent(vp, { 0, 0, 1, 1 })
    viewport.set_layer(vp, 1)
    viewport.set_clear_depth(vp, true)
    viewport.set_mode(vp, viewport.STRETCH_KEEP_ASPECT)
    viewport.set_auto_resize(vp, true)
    viewport.set_anchor(vp, anchor.LEFT)

    self.gui_canvas = canvas.new(self.arena, WIDTH, HEIGHT)
    self.vp = viewport.new(self.arena, texture.screen())
    -- viewport.set_extent(self.vp, { 0, 0, 1, 1 })
    viewport.set_texture(self.vp, canvas.texture(self.gui_canvas))
    viewport.set_mode(self.vp, viewport.STRETCH_KEEP_ASPECT)
    viewport.set_layer(self.vp, 2)
    viewport.set_auto_resize(vp, true)
    -- viewport.set_anchor(self.vp, anchor.TOP | anchor.LEFT)

    local gui_tex = texture.load(self.arena, "assets/GUI.png")
    local style = stylesheet.new(self.arena)
    stylesheet.set(style, stylesheet.BUTTON_PRESSED, gui_tex,
        { 113, 97, 30, 14 },
        { 115, 100, 26, 8 })
    stylesheet.set(style, stylesheet.BUTTON_RELEASED, gui_tex,
        { 113, 81, 30, 14 },
        { 115, 83, 26, 8 })
    stylesheet.set(style, stylesheet.BUTTON_HOVERED, gui_tex,
        { 113, 113, 30, 14 },
        { 115, 115, 26, 8 })
    stylesheet.set(style, stylesheet.CHECKBOX_CHECKED, gui_tex,
        { 97, 257, 14, 14 },
        { 99, 260, 10, 8 })
    stylesheet.set(style, stylesheet.CHECKBOX_UNCHECKED, gui_tex,
        { 81, 257, 14, 14 },
        { 99, 261, 10, 8 })
    stylesheet.set(style, stylesheet.CURSOR, gui_tex,
        { 52, 83, 8, 7 },
        { 52, 83, 8, 7 })

    self.gui = gui.new(self.arena, self.gui_canvas)
    gui.push_style(self.gui, style)

    local plane = mesh.new_plane(self.arena, 6.4, 4.8)
    local n = node.create(node.root())
    node.add(n, component.STATICMESH)
    node.add(n, component.TRANSFORM)
    staticmesh.set_mesh(n, plane)
    staticmesh.set_texture(n, canvas.texture(self.gui_canvas))
    transform.set_translation(n, { 10, 2, 0 })
    transform.set_rotation_euler(n, { -math.rad(95), 0, 0 })
end

function M:on_update()
    local position = transform.translation(self.camera.node)
    c = self.gui_canvas
    local h = 9
    canvas.text(c, 10, h * 1, time.date())
    canvas.text(c, 10, h * 2, string.format("time: %.2fs", time.elapsed()))
    canvas.text(c, 10, h * 3, string.format("xyz: %.2f %.2f %.2f", position.x, position.y, position.z))
    canvas.text(c, 10, h * 4,
        string.format("core mem: %s bc:%d",
            math.memhu(arena.memory_usage(arena.core())),
            arena.block_count(arena.core())))
    canvas.text(c, 10, h * 5,
        string.format("frame mem: %s bc:%d",
            math.memhu(arena.memory_usage(arena.frame())),
            arena.block_count(arena.frame())))
    canvas.text(c, 10, h * 6, string.format("nodes: %d", scene.count()))
    local gcur = input.cursor(0)
    local cur = viewport.to_local(self.vp, gcur)
    canvas.text(c, 10, h * 7, string.format("cursor: %.2f %.2f", cur.x, cur.y))
    canvas.text(c, 10, h * 8, string.format("fov: %d", self.camera.fov))
    local screen_size = texture.size(texture.screen())
    canvas.text(c, 10, h * 9, string.format("size: %dx%d", screen_size.x, screen_size.y))

    if self.gizmos.target then
        local tex = staticmesh.texture(self.gizmos.target)
        if tex then
            local size = texture.size(tex)
            local csize = canvas.size(c)
            canvas.blit(c, tex, vmath.box2i(csize.x - size.x, csize.y - size.y, size.x, size.y),
                vmath.box2i(0, 0, size.x, size.y))
        end
    end

    canvas.rectangle(c, cur.x * WIDTH - 1, cur.y * HEIGHT - 1, 5, 5)

    gui.button(self.gui, 200, 400, 100, 50)
    -- gui.button(self.gui, 400, 200, 100, 50)

    camera.reset_aspect(self.camera.node, self.vp)
    camera.reset_aspect(self.camera.top_node, self.vp)
    -- local aspect = WIDTH / HEIGHT
    -- camera.set_aspect(self.camera.node, aspect)
    -- camera.set_aspect(self.camera.top_node, aspect)

    local dir = camera.unproject(self.camera.node, vmath.vec2(0.5, 0.5))
    local dir = vmath.sub(dir, position) -- Convert position to direction
    self.gizmos:update(position, dir, "click")
end
