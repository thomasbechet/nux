local mod = MODULE

function mod:on_load()
    local files = {
        "conf.lua",
        "init.lua",
        "camera.lua",
        "camera.lua",
        "libs/inspect.lua",
        "assets/industrial.glb"
    }
    assert(nux.io.cart_begin("cart.bin", #files))
    for _, v in pairs(files) do
        assert(nux.io.write_cart_file(v))
    end
    assert(nux.io.cart_end())
end

return mod
