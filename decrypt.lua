local function base64Decode(s)
    local b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    s = s:gsub("[^"..b64.."=]", "")
    return (s:gsub(".", function(x)
        if x == "=" then return "" end
        local n = b64:find(x)-1
        return string.char(n)
    end):gsub("(..)(.)", function(a,b)
        return string.char((a:byte(1)*4 + a:byte(2)/16) % 256)
    end))
end

local function xorDecrypt(s, key)
    local out = {}
    for i=1,#s do
        out[i] = string.char(s:byte(i) ~ key:byte((i-1)%#key+1))
    end
    return table.concat(out)
end

function decrypt.process(ast)
    local function walk(node)
        if node.tag == "string" and node.text:match("^[A-Za-z0-9+/=]+$") then
            local decoded = base64Decode(node.text)
            if decoded:match("%w") then
                node.text = decoded
            end
        end
        for k, v in pairs(node) do
            if type(v) == "table" then walk(v) end
        end
    end
    walk(ast.root)
    return ast
end

return decrypt
