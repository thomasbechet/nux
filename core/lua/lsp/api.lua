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
io={}
---@return None
function io.cart_begin(...) end
---@return None
function io.cart_end(...) end
---@return None
function io.write_cart_file(...) end
controller={}
controller.MAX=4
controller.MODE_SELECTION=0
controller.MODE_CURSOR=1
name={}
name.MAX=64
disk={}
disk.MAX=8
inputmap={}
---@return None
function inputmap.new(...) end
---@return None
function inputmap.bind_key(...) end
---@return None
function inputmap.bind_mouse_button(...) end
---@return None
function inputmap.bind_mouse_axis(...) end
input={}
input.UNMAPPED=0
input.KEY=1
input.MOUSE_BUTTON=2
input.MOUSE_AXIS=3
input.GAMEPAD_BUTTON=4
input.GAMEPAD_AXIS=5
---@return None
function input.set_map(...) end
---@return None
function input.pressed(...) end
---@return None
function input.released(...) end
---@return None
function input.just_pressed(...) end
---@return None
function input.just_released(...) end
---@return None
function input.value(...) end
---@return None
function input.cursor(...) end
---@return None
function input.set_cursor(...) end
button={}
button.PRESSED=1
button.RELEASED=0
key={}
key.SPACE=0
key.APOSTROPHE=1
key.COMMA=2
key.MINUS=3
key.PERIOD=4
key.SLASH=5
key.NUM0=6
key.NUM1=7
key.NUM2=8
key.NUM3=9
key.NUM4=10
key.NUM5=11
key.NUM6=12
key.NUM7=13
key.NUM8=14
key.NUM9=15
key.SEMICOLON=16
key.EQUAL=17
key.A=18
key.B=19
key.C=20
key.D=21
key.E=22
key.F=23
key.G=24
key.H=25
key.I=26
key.J=27
key.K=29
key.L=30
key.M=31
key.N=32
key.O=33
key.P=34
key.Q=35
key.R=36
key.S=37
key.T=38
key.U=39
key.V=40
key.W=41
key.X=42
key.Y=43
key.Z=44
key.LEFT_BRACKET=45
key.BACKSLASH=46
key.RIGHT_BRACKET=47
key.GRAVE_ACCENT=48
key.ESCAPE=49
key.ENTER=50
key.TAB=51
key.BACKSPACE=52
key.INSERT=53
key.DELETE=54
key.RIGHT=55
key.LEFT=56
key.DOWN=57
key.UP=58
key.PAGE_UP=59
key.PAGE_DOWN=60
key.HOME=61
key.END=62
key.CAPS_LOCK=63
key.SCROLL_LOCK=64
key.NUM_LOCK=65
key.PRINT_SCREEN=66
key.PAUSE=67
key.F1=68
key.F2=69
key.F3=70
key.F4=71
key.F5=72
key.F6=73
key.F7=74
key.F8=75
key.F9=76
key.F10=77
key.F11=78
key.F12=79
key.F13=80
key.F14=81
key.F15=82
key.F16=83
key.F17=84
key.F18=85
key.F19=86
key.F20=87
key.F21=88
key.F22=89
key.F23=90
key.F24=91
key.F25=92
key.KP_0=93
key.KP_1=94
key.KP_2=95
key.KP_3=96
key.KP_4=97
key.KP_5=98
key.KP_6=99
key.KP_7=100
key.KP_8=101
key.KP_9=102
key.KP_DECIMAL=103
key.KP_DIVIDE=104
key.KP_MULTIPLY=105
key.KP_SUBTRACT=106
key.KP_ADD=107
key.KP_ENTER=108
key.KP_EQUAL=109
key.LEFT_SHIFT=110
key.LEFT_CONTROL=111
key.LEFT_ALT=112
key.LEFT_SUPER=113
key.RIGHT_SHIFT=114
key.RIGHT_CONTROL=115
key.RIGHT_ALT=116
key.RIGHT_SUPER=117
key.MENU=118
mouse={}
mouse.BUTTON_LEFT=0
mouse.BUTTON_RIGHT=1
mouse.BUTTON_MIDDLE=2
mouse.WHEEL_UP=3
mouse.WHEEL_DOWN=4
mouse.MOTION_RIGHT=0
mouse.MOTION_LEFT=1
mouse.MOTION_DOWN=2
mouse.MOTION_UP=3
mouse.SCROLL_UP=4
mouse.SCROLL_DOWN=5
gamepad={}
gamepad.A=0
gamepad.X=1
gamepad.Y=2
gamepad.B=3
gamepad.DPAD_UP=4
gamepad.DPAD_DOWN=5
gamepad.DPAD_LEFT=6
gamepad.DPAD_RIGHT=7
gamepad.SHOULDER_LEFT=8
gamepad.SHOULDER_RIGHT=9
gamepad.LSTICK_LEFT=0
gamepad.LSTICK_RIGHT=1
gamepad.LSTICK_UP=2
gamepad.LSTICK_DOWN=3
gamepad.RSTICK_LEFT=4
gamepad.RSTICK_RIGHT=5
gamepad.RSTICK_UP=6
gamepad.RSTICK_DOWN=7
gamepad.LTRIGGER=8
gamepad.RTRIGGER=9
cursor={}
cursor.UP=0
cursor.DOWN=1
cursor.LEFT=2
cursor.RIGHT=3
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
function viewport.set_auto_resize(...) end
---@return None
function viewport.set_camera(...) end
---@return None
function viewport.set_texture(...) end
---@return None
function viewport.get_normalized_viewport(...) end
---@return None
function viewport.to_global(...) end
---@return None
function viewport.to_local(...) end
texture={}
texture.IMAGE_RGBA=0
texture.IMAGE_INDEX=1
texture.RENDER_TARGET=2
texture.WRAP_CLAMP=0
texture.WRAP_REPEAT=1
texture.WRAP_MIRROR=2
---@return None
function texture.new(...) end
---@return None
function texture.load(...) end
---@return None
function texture.screen(...) end
---@return None
function texture.get_size(...) end
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
function mesh.new_plane(...) end
---@return None
function mesh.update_bounds(...) end
---@return None
function mesh.bounds_min(...) end
---@return None
function mesh.bounds_max(...) end
---@return None
function mesh.set_origin(...) end
---@return None
function mesh.transform(...) end
---@return None
function mesh.size(...) end
canvas={}
---@return None
function canvas.new(...) end
---@return None
function canvas.get_texture(...) end
---@return None
function canvas.get_size(...) end
---@return None
function canvas.set_clear_color(...) end
---@return None
function canvas.set_wrap_mode(...) end
---@return None
function canvas.text(...) end
---@return None
function canvas.rectangle(...) end
---@return None
function canvas.blit(...) end
---@return None
function canvas.blit_sliced(...) end
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
stylesheet={}
stylesheet.BUTTON_PRESSED=0
stylesheet.BUTTON_RELEASED=1
stylesheet.BUTTON_HOVERED=2
stylesheet.CHECKBOX_CHECKED=3
stylesheet.CHECKBOX_UNCHECKED=4
stylesheet.CURSOR=5
---@return None
function stylesheet.new(...) end
---@return None
function stylesheet.set(...) end
gui={}
---@return None
function gui.new(...) end
---@return None
function gui.push_style(...) end
---@return None
function gui.pop_style(...) end
---@return None
function gui.button(...) end
