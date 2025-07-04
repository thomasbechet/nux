local nux = nux

function nux.node.instantiate(scene, tab, parent)
    local loaders

    local function load_node(n, t)
        for k, v in pairs(t) do
            local load = loaders[k]
            if load then
                load(n, v)
            else
                nux.node.instantiate(scene, v, n)
            end
        end
    end

    -- Component loaders
    loaders = {
        template = function(n, v)
            load_node(n, v)
        end,
        transform = function(n, v)
            nux.transform.add(n)
            if v.translation then
                nux.transform.set_translation(n, table.unpack(v.translation))
            end
            if v.scale then
                nux.transform.set_scale(n, table.unpack(v.scale))
            end
        end,
        camera = function(n, v)
            nux.camera.add(n)
            if v.fov then
                nux.camera.set_fov(n, v.fov)
            end
        end,
        staticmesh = function(n, v)
            nux.staticmesh.add(n)
            if v.mesh then
                nux.staticmesh.set_mesh(n, v.mesh)
            end
        end,
    }

    local n = nux.node.new(scene)
    if parent then
        nux.node.set_parent(n, parent)
    end
    load_node(n, tab)
    return n
end

function nux.scene.load(tab)
    local scene = nux.scene.new()
    nux.node.instantiate(scene, tab, nil)
    return scene
end
