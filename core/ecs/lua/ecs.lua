local nux = nux

function nux.ecs.instantiate(tab, parent)
    local loaders

    local function load_entity(n, t)
        for k, v in pairs(t) do
            local load = loaders[k]
            if load then
                load(n, v)
            else
                nux.ecs.instantiate(v, n)
            end
        end
    end

    -- Component loaders
    loaders = {
        template = function(n, v)
            load_entity(n, v)
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

    local e = nux.ecs.create()
    load_entity(e, tab)
    if parent then
        nux.transform.set_parent(e, parent)
    end
    return e
end

function nux.ecs.load(tab)
    local ecs = nux.ecs.new(100)
    local prev = nux.ecs.get_active()
    nux.ecs.set_active(ecs)
    nux.ecs.instantiate(tab, nil)
    nux.ecs.set_active(prev)
    return ecs
end
