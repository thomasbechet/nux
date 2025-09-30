function ecs.instantiate(tab, parent)
    local loaders

    local function load_entity(n, t)
        for k, v in pairs(t) do
            local load = loaders[k]
            if load then
                load(n, v)
            else
                ecs.instantiate(v, n)
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

    local e = ecs.create(ecs.root())
    if parent then
        e = ecs.create(parent)
    else
        e = ecs.create(ecs.root())
    end
    load_entity(e, tab)
    return e
end

function ecs.load(tab)
    local e = ecs.new(100)
    local prev = ecs.get_active()
    ecs.set_active(e)
    ecs.instantiate(tab, nil)
    ecs.set_active(prev)
    return e
end
