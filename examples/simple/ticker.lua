function M:on_load()
    self.time = 0
    self.interval = 1
    self.event = event.new(resource.find("core_arena"))
end

function M:on_update()
    self.time = self.time + time.delta()
    if self.time > self.interval then
        event.emit(self.event, "test")
        self.time = 0
    end
end
