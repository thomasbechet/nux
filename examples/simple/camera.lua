local nux = nux
local camera = MODULE

local function init()
    camera.entity = nil
    camera.speed = 10
    camera.fast_speed = 20
    camera.fov = 90
    camera.fast_fov = 92
    camera.pitch = 0
    camera.yaw = 0
    local cam = nux.ecs.create()
    nux.camera.add(cam)
    nux.transform.add(cam)
    nux.transform.set_translation(cam, { 13, 15, 10 })
    return cam
end

function camera.update()
    if not camera.entity then
        camera.entity = init()
    end

    local speed = camera.speed
    local fov = camera.fov
    local e = camera.entity

    if nux.button.pressed(0, nux.button.LB) then
        speed = camera.fast_speed
        fov = camera.fast_fov
    end
    nux.camera.set_fov(e, fov)

    local mx = nux.axis.value(0, nux.axis.LEFTX)
    local mz = nux.axis.value(0, nux.axis.LEFTY)
    local my = 0
    if nux.button.pressed(0, nux.button.A) then
        my = 1
    elseif nux.button.pressed(0, nux.button.B) then
        my = -1
    end
    local rx = nux.axis.value(0, nux.axis.RIGHTX)
    local ry = nux.axis.value(0, nux.axis.RIGHTY)

    -- Translation
    local forward = nux.transform.forward(e)
    local left = nux.transform.left(e)
    local dt = nux.time.delta()
    -- Forward
    local dir = forward * mz * dt * speed
    -- Left
    dir = dir - left * mx * dt * speed
    -- Up
    dir.y = dir.y + my * dt * speed
    local position = nux.transform.get_translation(e)
    nux.transform.set_translation(e, position + dir)

    -- Rotation
    if rx ~= 0 then
        camera.yaw = camera.yaw + rx * nux.time.delta() * 100
    end
    if ry ~= 0 then
        camera.pitch = camera.pitch - ry * nux.time.delta() * 100
    end
    camera.pitch = math.clamp(camera.pitch, -90, 90)
    nux.transform.set_rotation_euler(e, nux.vmath.vec3(-math.rad(camera.pitch), -math.rad(camera.yaw), 0))
    nux.camera.set_far(e, 1000)
    nux.camera.set_near(e, 0.1)
end

return camera
