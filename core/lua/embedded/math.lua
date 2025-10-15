function math.clamp(n, low, high)
    return math.min(math.max(n, low), high)
end

function math.memhu(size)
    local i = 1
    while size > 1024 do
        i = i + 1
        size = size / 1024
    end
    local units = { "B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" }
    return string.format("%.02f%s", size, units[i])
end
