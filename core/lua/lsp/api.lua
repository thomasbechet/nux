core={}
core.SUCCESS=1
core.FAILURE=0
---@return integer
function core.stat(...) end
---@return integer
function core.random(...) end
time={}
---@return number
function time.elapsed(...) end
---@return number
function time.delta(...) end
---@return integer
function time.frame(...) end
---@return None
function time.timestamp(...) end
arena={}
---@return integer
function arena.new(...) end
---@return nil
function arena.reset(...) end
---@return integer
function arena.memory_usage(...) end
---@return integer
function arena.memory_capacity(...) end
---@return integer
function arena.block_count(...) end
event={}
---@return integer
function event.new(...) end
resource={}
---@return None
function resource.get_path(...) end
---@return nil
function resource.set_name(...) end
---@return None
function resource.get_name(...) end
---@return integer
function resource.find(...) end
error={}
error.NONE=0
error.OUT_OF_MEMORY=1
error.INVALID_TEXTURE_SIZE=4
error.WASM_RUNTIME=8
error.CART_EOF=10
error.CART_MOUNT=11
stat={}
stat.FPS=0
stat.SCREEN_WIDTH=1
stat.SCREEN_HEIGHT=2
stat.TIMESTAMP=3
stat.MAX=4
log={}
log.DEBUG=4
log.INFO=3
log.WARNING=2
log.ERROR=1
---@return nil
function log.set_level(...) end
button={}
button.MAX=10
button.A=1 << 0
button.X=1 << 1
button.Y=1 << 2
button.B=1 << 3
button.UP=1 << 4
button.DOWN=1 << 5
button.LEFT=1 << 6
button.RIGHT=1 << 7
button.LB=1 << 8
button.RB=1 << 9
---@return integer
function button.state(...) end
---@return boolean
function button.pressed(...) end
---@return boolean
function button.released(...) end
---@return boolean
function button.just_pressed(...) end
---@return boolean
function button.just_released(...) end
axis={}
axis.MAX=6
axis.LEFTX=0
axis.LEFTY=1
axis.RIGHTX=2
axis.RIGHTY=3
axis.RT=4
axis.LT=5
---@return number
function axis.value(...) end
cursor={}
---@return number
function cursor.x(...) end
---@return number
function cursor.y(...) end
---@return nil
function cursor.set(...) end
io={}
---@return integer
function io.cart_begin(...) end
---@return integer
function io.cart_end(...) end
---@return integer
function io.write_cart_file(...) end
controller={}
controller.MAX=4
name={}
name.MAX=64
disk={}
disk.MAX=8
lua={}
---@return integer
function lua.load(...) end
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
texture={}
texture.IMAGE_RGBA=0
texture.IMAGE_INDEX=1
texture.RENDER_TARGET=2
---@return integer
function texture.new(...) end
---@return nil
function texture.blit(...) end
mesh={}
---@return integer
function mesh.new(...) end
---@return integer
function mesh.new_cube(...) end
---@return nil
function mesh.update_bounds(...) end
---@return Userdata
function mesh.bounds_min(...) end
---@return Userdata
function mesh.bounds_max(...) end
canvas={}
canvas.WIDTH=640
canvas.HEIGHT=400
---@return integer
function canvas.new(...) end
---@return integer
function canvas.get_texture(...) end
---@return nil
function canvas.set_layer(...) end
---@return nil
function canvas.set_clear_color(...) end
---@return nil
function canvas.text(...) end
---@return nil
function canvas.rectangle(...) end
graphics={}
---@return nil
function graphics.draw_line_tr(...) end
---@return nil
function graphics.draw_line(...) end
---@return nil
function graphics.draw_dir(...) end
palette={}
palette.SIZE=256
colormap={}
colormap.SIZE=256
primitive={}
primitive.TRIANGLES=0
primitive.LINES=1
primitive.POINTS=2
vertex={}
vertex.TRIANGLES=0
vertex.LINES=1
vertex.POINTS=2
vertex.POSITION=1 << 0
vertex.UV=1 << 1
vertex.COLOR=1 << 2
vertex.INDICES=1 << 3
vertex.V3F=0
vertex.V3F_T2F=1
vertex.V3F_T2F_N3F=2
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
