local fluid = require("fluid")

local elements = fluid.element.list()
local window = 0
for k, v in pairs(elements) do
    if v == "MainWindow" then
        window = round(tonumber(k))
    end
end

local widget = require("widget")

-- Closing the main window through its widget component
print("Main Window Entity :", window)