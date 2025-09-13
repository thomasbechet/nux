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
