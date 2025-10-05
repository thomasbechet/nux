core={}
core.SUCCESS=1
core.FAILURE=0
---@return None
function core.stat(...) end
---@return None
function core.random(...) end
---@return None
function core.random01(...) end
time={}
---@return None
function time.elapsed(...) end
---@return None
function time.delta(...) end
---@return None
function time.frame(...) end
---@return None
function time.timestamp(...) end
arena={}
---@return None
function arena.new(...) end
---@return None
function arena.reset(...) end
---@return None
function arena.memory_usage(...) end
---@return None
function arena.memory_capacity(...) end
---@return None
function arena.block_count(...) end
---@return None
function arena.core(...) end
---@return None
function arena.frame(...) end
resource={}
---@return None
function resource.path(...) end
---@return None
function resource.set_name(...) end
---@return None
function resource.name(...) end
---@return None
function resource.arena(...) end
---@return None
function resource.find(...) end
log={}
log.DEBUG=4
log.INFO=3
log.WARNING=2
log.ERROR=1
---@return None
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
---@return None
function button.state(...) end
---@return None
function button.pressed(...) end
---@return None
function button.released(...) end
---@return None
function button.just_pressed(...) end
---@return None
function button.just_released(...) end
axis={}
axis.MAX=6
axis.LEFTX=0
axis.LEFTY=1
axis.RIGHTX=2
axis.RIGHTY=3
axis.RT=4
axis.LT=5
---@return None
function axis.value(...) end
cursor={}
---@return None
function cursor.get(...) end
---@return None
function cursor.set(...) end
---@return None
function cursor.x(...) end
---@return None
function cursor.y(...) end
io={}
---@return None
function io.cart_begin(...) end
---@return None
function io.cart_end(...) end
---@return None
function io.write_cart_file(...) end
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
controller={}
controller.MAX=4
name={}
name.MAX=64
disk={}
disk.MAX=8
lua={}
---@return None
function lua.load(...) end
transform={}
---@return None
function transform.add(...) end
---@return None
function transform.remove(...) end
---@return None
function transform.get_local_translation(...) end
---@return None
function transform.get_local_rotation(...) end
---@return None
function transform.get_local_scale(...) end
---@return None
function transform.get_translation(...) end
---@return None
function transform.get_rotation(...) end
---@return None
function transform.get_scale(...) end
---@return None
function transform.set_translation(...) end
---@return None
function transform.set_rotation(...) end
---@return None
function transform.set_rotation_euler(...) end
---@return None
function transform.set_scale(...) end
---@return None
function transform.set_ortho(...) end
---@return None
function transform.forward(...) end
---@return None
function transform.backward(...) end
---@return None
function transform.left(...) end
---@return None
function transform.right(...) end
---@return None
function transform.up(...) end
---@return None
function transform.down(...) end
---@return None
function transform.rotate(...) end
---@return None
function transform.rotate_x(...) end
---@return None
function transform.rotate_y(...) end
---@return None
function transform.rotate_z(...) end
---@return None
function transform.look_at(...) end
query={}
---@return None
function query.new(...) end
---@return None
function query.new_any(...) end
---@return None
function query.includes(...) end
---@return None
function query.excludes(...) end
---@return None
function query.next(...) end
scene={}
---@return None
function scene.new(...) end
---@return None
function scene.set_active(...) end
---@return None
function scene.active(...) end
---@return None
function scene.count(...) end
---@return None
function scene.clear(...) end
---@return None
function scene.load_gltf(...) end
node={}
---@return None
function node.create(...) end
---@return None
function node.delete(...) end
---@return None
function node.valid(...) end
---@return None
function node.root(...) end
---@return None
function node.parent(...) end
---@return None
function node.set_parent(...) end
---@return None
function node.sibling(...) end
---@return None
function node.child(...) end
---@return None
function node.remove(...) end
---@return None
function node.has(...) end
---@return None
function node.instantiate(...) end
texture={}
texture.IMAGE_RGBA=0
texture.IMAGE_INDEX=1
texture.RENDER_TARGET=2
---@return None
function texture.new(...) end
---@return None
function texture.blit(...) end
mesh={}
---@return None
function mesh.new(...) end
---@return None
function mesh.new_cube(...) end
---@return None
function mesh.update_bounds(...) end
---@return None
function mesh.bounds_min(...) end
---@return None
function mesh.bounds_max(...) end
canvas={}
canvas.WIDTH=640
canvas.HEIGHT=400
---@return None
function canvas.new(...) end
---@return None
function canvas.get_texture(...) end
---@return None
function canvas.set_layer(...) end
---@return None
function canvas.set_clear_color(...) end
---@return None
function canvas.text(...) end
---@return None
function canvas.rectangle(...) end
graphics={}
---@return None
function graphics.draw_line_tr(...) end
---@return None
function graphics.draw_line(...) end
---@return None
function graphics.draw_dir(...) end
camera={}
---@return None
function camera.add(...) end
---@return None
function camera.remove(...) end
---@return None
function camera.set_fov(...) end
---@return None
function camera.set_near(...) end
---@return None
function camera.set_far(...) end
staticmesh={}
---@return None
function staticmesh.has(...) end
---@return None
function staticmesh.add(...) end
---@return None
function staticmesh.remove(...) end
---@return None
function staticmesh.set_mesh(...) end
---@return None
function staticmesh.get_mesh(...) end
---@return None
function staticmesh.set_texture(...) end
---@return None
function staticmesh.get_texture(...) end
---@return None
function staticmesh.set_colormap(...) end
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
---@return None
function rigidbody.add(...) end
---@return None
function rigidbody.remove(...) end
---@return None
function rigidbody.set_velocity(...) end
collider={}
collider.SPHERE=0
collider.AABB=1
---@return None
function collider.add_sphere(...) end
---@return None
function collider.add_aabb(...) end
---@return None
function collider.remove(...) end
physics={}
---@return None
function physics.raycast(...) end
