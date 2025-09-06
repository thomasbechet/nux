local mod = MODULE

function mod:init()
    mod.state = 1
end

function mod.hello()
    mod.state = mod.state + 0
end

function mod:reload()
    print("reload!")
end

return mod
