rigidbody={}
---@return nil
function rigidbody.add(...) end
---@return nil
function rigidbody.remove(...) end
---@return nil
function rigidbody.set_velocity(...) end
collider={}
collider.SPHERE=0
collider.AABB=1
---@return nil
function collider.add_sphere(...) end
---@return nil
function collider.add_aabb(...) end
---@return nil
function collider.remove(...) end
physics={}
---@return Userdata
function physics.raycast(...) end
