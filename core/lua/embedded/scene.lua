function node.instantiate(tab, parent)
    local loaders

    local function load_entity(n, t)
        for k, v in pairs(t) do
            local load = loaders[k]
            if load then
                load(n, v)
            else
                node.instantiate(v, n)
            end
        end
    end

    -- Component loaders
    loaders = {
        template = function(n, v)
            load_entity(n, v)
        end,
        transform = function(n, v)
            transform.add(n)
            if v.translation then
                transform.set_translation(n, v.translation)
            end
            if v.scale then
                transform.set_scale(n, v.scale)
            end
        end,
        camera = function(n, v)
            camera.add(n)
            if v.fov then
                camera.set_fov(n, v.fov)
            end
        end,
        staticmesh = function(n, v)
            staticmesh.add(n)
            if v.mesh then
                staticmesh.set_mesh(n, v.mesh)
            end
        end,
    }

    local e = node.create(node.root())
    if parent then
        e = node.create(parent)
    else
        e = node.create(node.root())
    end
    load_entity(e, tab)
    return e
end

function scene.load(tab)
    local n = scene.new(100)
    local prev = scene.get_active()
    scene.set_active(n)
    node.instantiate(tab, nil)
    scene.set_active(prev)
    return n
end
