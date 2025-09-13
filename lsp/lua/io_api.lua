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
