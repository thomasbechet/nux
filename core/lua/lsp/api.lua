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
color={}
color.TRANSPARENT=0
color.WHITE=1
color.RED=2
color.GREEN=3
color.BLUE=4
color.BACKGROUND=5
---@return None
function color.rgba(...) end
---@return None
function color.hex(...) end
---@return None
function color.to_hex(...) end
---@return None
function color.to_srgb(...) end
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
function transform.get_matrix(...) end
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
function node.add(...) end
---@return None
function node.remove(...) end
---@return None
function node.has(...) end
---@return None
function node.instantiate(...) end
component={}
component.TRANSFORM=1
component.CAMERA=2
component.STATICMESH=3
component.RIGIDBODY=4
component.COLLIDER=5
component.MAX=16
viewport={}
viewport.HIDDEN=0
viewport.FIXED=1
viewport.FIXED_BEST_FIT=2
viewport.STRETCH_KEEP_ASPECT=3
viewport.STRETCH=4
---@return None
function viewport.new(...) end
---@return None
function viewport.set_mode(...) end
---@return None
function viewport.set_extent(...) end
---@return None
function viewport.set_anchor(...) end
---@return None
function viewport.set_layer(...) end
---@return None
function viewport.set_clear_depth(...) end
---@return None
function viewport.set_camera(...) end
---@return None
function viewport.set_texture(...) end
---@return None
function viewport.get_target_size(...) end
---@return None
function viewport.get_global_extent(...) end
---@return None
function viewport.to_global(...) end
---@return None
function viewport.to_local(...) end
texture={}
texture.IMAGE_RGBA=0
texture.IMAGE_INDEX=1
texture.RENDER_TARGET=2
---@return None
function texture.new(...) end
---@return None
function texture.screen(...) end
palette={}
palette.SIZE=256
---@return None
function palette.new(...) end
---@return None
function palette.default(...) end
---@return None
function palette.set_active(...) end
---@return None
function palette.set_color(...) end
---@return None
function palette.get_color(...) end
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
---@return None
function mesh.set_origin(...) end
canvas={}
---@return None
function canvas.new(...) end
---@return None
function canvas.get_texture(...) end
---@return None
function canvas.set_clear_color(...) end
---@return None
function canvas.text(...) end
---@return None
function canvas.rectangle(...) end
graphics={}
---@return None
function graphics.begin_state(...) end
---@return None
function graphics.end_state(...) end
---@return None
function graphics.reset_state(...) end
---@return None
function graphics.draw_line(...) end
---@return None
function graphics.draw_dir(...) end
---@return None
function graphics.set_layer(...) end
---@return None
function graphics.set_color(...) end
---@return None
function graphics.set_transform(...) end
---@return None
function graphics.set_transform_identity(...) end
camera={}
---@return None
function camera.set_fov(...) end
---@return None
function camera.set_near(...) end
---@return None
function camera.set_far(...) end
---@return None
function camera.set_aspect(...) end
---@return None
function camera.reset_aspect(...) end
---@return None
function camera.set_ortho(...) end
---@return None
function camera.set_ortho_size(...) end
---@return None
function camera.get_projection(...) end
---@return None
function camera.unproject(...) end
---@return None
function camera.set_render_mask(...) end
---@return None
function camera.get_render_mask(...) end
staticmesh={}
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
---@return None
function staticmesh.set_render_layer(...) end
---@return None
function staticmesh.get_render_layer(...) end
---@return None
function staticmesh.set_draw_bounds(...) end
colormap={}
colormap.SIZE=256
layer={}
layer.DEFAULT=0x1
primitive={}
primitive.TRIANGLES=0
primitive.LINES=1
primitive.POINTS=2
vertex={}
vertex.TRIANGLES=0
vertex.LINES=1
vertex.POINTS=2
vertex.POSITION=1 << 0
vertex.TEXCOORD=1 << 1
vertex.COLOR=1 << 2
anchor={}
anchor.CENTER=0
anchor.TOP=1 << 1
anchor.BOTTOM=1 << 2
anchor.LEFT=1 << 3
anchor.RIGHT=1 << 4
rigidbody={}
---@return None
function rigidbody.set_velocity(...) end
collider={}
collider.SPHERE=0
collider.AABB=1
---@return None
function collider.set_sphere(...) end
---@return None
function collider.set_aabb(...) end
physics={}
---@return None
function physics.raycast(...) end
---@return None
function physics.set_ground_height(...) end
