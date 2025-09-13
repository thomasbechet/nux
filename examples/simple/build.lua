function M:on_load()
    local files = {
        "conf.lua",
        "init.lua",
        "camera.lua",
        "camera.lua",
        "libs/inspect.lua",
        "assets/industrial.glb"
    }
    assert(io.cart_begin("cart.bin", #files))
    for _, v in pairs(files) do
        assert(io.write_cart_file(v))
    end
    assert(io.cart_end())
end
