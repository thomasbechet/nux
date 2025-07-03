function nux.scene.parse(tab)
    local nodes = {}
    local loaders
    local scene

    local function load_nodes(t, p)
        -- Create nodes
        for k in pairs(t) do
            local n = nux.node.new(scene)
            nodes[k] = n
            if p then
                nux.node.set_parent(n, p)
            end
        end

        -- Add components
        for k, v in pairs(t) do
            local n = nodes[k]
            for kc, vc in pairs(v) do
                local load = loaders[kc]
                if load then
                    load(n, vc)
                end
            end
        end
    end

    -- Component loaders
    loaders = {
        children = function(n, c)
            load_nodes(c, n)
        end,
        transform = function(n, c)
            nux.transform.add(n)
            if c.translation then
                nux.transform.set_translation(n, table.unpack(c.translation))
            end
            if c.scale then
                nux.transform.set_scale(n, table.unpack(c.scale))
            end
        end,
        camera = function(n, c)
            nux.camera.add(n)
            if c.fov then
                nux.camera.set_fov(n, c.fov)
            end
        end,
        staticmesh = function(n, c)
            nux.staticmesh.add(n)
            if c.mesh then
                nux.staticmesh.set_mesh(n, c.mesh)
            end
        end,
    }

    -- Create scene
    scene = nux.scene.new()
    load_nodes(tab, nil)
    return scene
end
