function time.date_ymd()
    local z = time.timestamp() // 86400 -- days since epoch
    z = z + 719468
    local era
    if z >= 0 then
        era = z
    else
        era = z - 146096
    end
    era = era // 146097
    local doe = z - era * 146097                                          -- [0, 146096]
    local yoe = (doe - doe // 1460 + doe // 36524 - doe // 146096) // 365 -- [0, 399]
    local doy = doe - (365 * yoe + yoe // 4 - yoe // 100)                 -- [0, 365]
    local mp = (5 * doy + 2) // 153                                       -- [0, 11]
    local d = doy - (153 * mp + 2) // 5 + 1                               -- [1, 31]
    local m                                                               -- [1, 12]
    if mp < 10 then
        m = mp + 3
    else
        m = mp - 9
    end
    local y = yoe + era * 400
    if m <= 2 then
        y = y + 1
    end
    return y, m, d
end

function time.date_hms()
    local z = time.timestamp() % 86400
    local h = z // 3600
    local m = (z % 3600) // 60
    local s = z % 60
    return h, m, s
end

function time.date()
    local y, mm, d = time.date_ymd()
    local h, m, s = time.date_hms()
    return string.format("%02d-%02d-%02d %02d:%02d:%02d", y, mm, d, h, m, s)
end
