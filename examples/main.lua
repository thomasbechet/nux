local inspect = require("inspect")
local module = {}

function module.hello()
    print("Hello")
    print(nux.frame())
end

module.hello()
-- print(inspect(nux))
return module
