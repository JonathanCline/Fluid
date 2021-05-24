local fluid = require("fluid")

print("Found Elements: \n")
local elements = fluid.element.list()

local window = 0
for k, v in pairs(elements) do
    print(k, v)
    if v == "MainWindow" then
        window = round(tonumber(k))
    end
end
print(window)

local widget = require("widget")
widget.close(window)
