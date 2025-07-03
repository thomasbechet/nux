function nux.scene.parse(tab)
    local nodes = {}

    -- Component loaders
    local loaders = {
        transform = function(e, v)
            nux.transform.add(e)
            if v.translation then
                nux.transform.set_translation(e, table.unpack(v.translation))
            end
            if v.scale then
                nux.transform.set_scale(e, table.unpack(v.scale))
            end
            if type(v.parent) == "number" then
                nux.transform.set_parent(e, v.parent)
            elseif type(v.parent) == "string" then
                nux.transform.set_parent(e, nodes[v.parent])
            end
        end,
        camera = function(e, v)
            nux.camera.add(e)
            if v.fov then
                nux.camera.set_fov(e, v.fov)
            end
        end,
        staticmesh = function(e, v)
            nux.staticmesh.add(e)
            if v.mesh then
                nux.staticmesh.set_mesh(e, v.mesh)
            end
        end
    }

    -- Create scene
    local s = nux.scene.new()

    -- Create nodes
    for k in pairs(tab) do
        local n = nux.node.new(s)
        nodes[k] = n
    end

    -- Add components
    for k, v in pairs(tab) do
        local n = nodes[k]
        for kc, vc in pairs(v) do
            local load = loaders[kc]
            if load then
                load(n, vc)
            end
        end
    end

    return s
end
