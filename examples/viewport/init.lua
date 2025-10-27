function M:on_load()
    local a = arena.core()

    self.canvas = canvas.new(a, 500, 500)

    local vp = viewport.new(a, texture.screen())
    viewport.set_texture(vp, canvas.get_texture(self.canvas))
    viewport.set_mode(vp, viewport.HIDDEN)
end

function M:on_update()
    canvas.rectangle(self.canvas, 0, 0, 500, 500)
end
