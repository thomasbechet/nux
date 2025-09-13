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
