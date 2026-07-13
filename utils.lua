function utils.merge(t1, t2)
    for k,v in pairs(t2) do t1[k]=v end
    return t1
end

function utils.clone(t)
    local r = {}
    for k,v in pairs(t) do r[k]=v end
    return r
end

function utils.isString(s)
    return type(s)=="string"
end

function utils.split(str, sep)
    local parts = {}
    for part in string.gmatch(str, "([^"..sep.."]+)") do
        parts[#parts+1] = part
    end
    return parts
end

return utils
