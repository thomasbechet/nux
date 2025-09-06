local mod = MODULE

function mod:init()
    print("load mod")
    mod.state = 1
end

function mod.hello()
    print(mod.state)
    mod.state = mod.state + 0
end

return mod
