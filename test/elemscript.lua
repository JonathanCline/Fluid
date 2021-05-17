
local fluid = fluid
assert(fluid, "fluid library not defined")
local entity = fluid.entity
local component = fluid.component
local element = component.element

local makinpancakes = entity.new()
fluid.add_component(makinpancakes, fluid.ctElement)

element.set_name(makinpancakes, "SCP")
print(element.get_name(makinpancakes))

fluid.yield()
print(entity.has(makinpancakes, fluid.ctElement))

