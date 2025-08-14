function nux.init()
    assert(nux.io.cart_begin("cart.bin", 4))
    assert(nux.io.write_cart_file("init.lua"))
    assert(nux.io.write_cart_file("inspect.lua"))
    assert(nux.io.write_cart_file("assets/industrial.glb"))
    assert(nux.io.cart_end())
end
