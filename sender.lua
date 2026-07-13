local fs = require("@std/fs")

local Sender = {}

function Sender.send(data)
    local json = require("@lune/serde").encode("json", data)
    local outFile = "deobf_output.json"
    if fs.writeFile then
        fs.writeFile(outFile, json)
    else
        print(json)
    end
    return json
end

return Sender
