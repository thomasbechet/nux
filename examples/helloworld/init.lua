function M:on_load()
    -- create screen canvas
    self.canvas = canvas.new(arena.core(), 1000, 500)
    local vp = viewport.new(arena.core(), graphics.screen_target())
    viewport.set_texture(vp, canvas.get_texture(self.canvas))
end

function M:on_update()
    -- canvas.set_clear_color(self.canvas, color.BACKGROUND)
    canvas.text(self.canvas, 10, 10, "hello world")
end
