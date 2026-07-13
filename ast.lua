local parser = require("@std/syntax/parser")
local Ast = {}

function Ast.parse(source)
    return parser.parse(source)
end

function Ast.getRaw(node)
    if node.tag == "number" then return node.value, true end
    if node.tag == "string" then return node.text, true end
    if node.tag == "boolean" then return node.value, true end
    if node.tag == "nil" then return nil, true end
    if node.tag == "binary" then
        local l, ls = Ast.getRaw(node.lhsoperand)
        local r, rs = Ast.getRaw(node.rhsoperand)
        if ls and rs then
            local op = node.operator.text
            if op == "+" then return l + r, true
            elseif op == "-" then return l - r, true
            elseif op == "*" then return l * r, true
            elseif op == "/" then return l / r, true
            elseif op == ".." then return l .. r, true
            elseif op == "==" then return l == r, true
            elseif op == "~=" then return l ~= r, true
            elseif op == "and" then return l and r, true
            elseif op == "or" then return l or r, true
            end
        end
    end
    if node.tag == "unary" then
        local v, s = Ast.getRaw(node.operand)
        if s then
            if node.operator.text == "-" then return -v, true
            elseif node.operator.text == "not" then return not v, true
            elseif node.operator.text == "#" then return #v, true
            end
        end
    end
    return nil, false
end

function Ast.makeNumber(value)
    return { tag = "number", value = value }
end

function Ast.makeString(value)
    return { tag = "string", text = value }
end

return Ast
