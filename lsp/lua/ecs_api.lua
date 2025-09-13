transform={}
---@return nil
function transform.add(...) end
---@return nil
function transform.remove(...) end
---@return nil
function transform.set_parent(...) end
---@return None
function transform.get_parent(...) end
---@return Userdata
function transform.get_local_translation(...) end
---@return Userdata
function transform.get_local_rotation(...) end
---@return Userdata
function transform.get_local_scale(...) end
---@return Userdata
function transform.get_translation(...) end
---@return Userdata
function transform.get_rotation(...) end
---@return Userdata
function transform.get_scale(...) end
---@return nil
function transform.set_translation(...) end
---@return nil
function transform.set_rotation(...) end
---@return nil
function transform.set_rotation_euler(...) end
---@return nil
function transform.set_scale(...) end
---@return nil
function transform.set_ortho(...) end
---@return Userdata
function transform.forward(...) end
---@return Userdata
function transform.backward(...) end
---@return Userdata
function transform.left(...) end
---@return Userdata
function transform.right(...) end
---@return Userdata
function transform.up(...) end
---@return Userdata
function transform.down(...) end
---@return nil
function transform.rotate(...) end
---@return nil
function transform.rotate_x(...) end
---@return nil
function transform.rotate_y(...) end
---@return nil
function transform.rotate_z(...) end
---@return nil
function transform.look_at(...) end
camera={}
---@return nil
function camera.add(...) end
---@return nil
function camera.remove(...) end
---@return nil
function camera.set_fov(...) end
---@return nil
function camera.set_near(...) end
---@return nil
function camera.set_far(...) end
staticmesh={}
---@return nil
function staticmesh.add(...) end
---@return nil
function staticmesh.remove(...) end
---@return nil
function staticmesh.set_mesh(...) end
---@return nil
function staticmesh.set_texture(...) end
---@return nil
function staticmesh.set_colormap(...) end
ecs={}
---@return integer
function ecs.new_iter(...) end
---@return nil
function ecs.includes(...) end
---@return nil
function ecs.excludes(...) end
---@return integer
function ecs.next(...) end
---@return integer
function ecs.new(...) end
---@return integer
function ecs.set_active(...) end
---@return integer
function ecs.load_gltf(...) end
---@return None
function ecs.create(...) end
---@return nil
function ecs.create_at(...) end
---@return nil
function ecs.delete(...) end
---@return boolean
function ecs.valid(...) end
---@return integer
function ecs.count(...) end
---@return nil
function ecs.clear(...) end
---@return nil
function ecs.remove(...) end
---@return boolean
function ecs.has(...) end
