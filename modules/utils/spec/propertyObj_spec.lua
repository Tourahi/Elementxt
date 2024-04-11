local PO = assert(require("propertyObj"))
local moon = assert(require("moon"))
local dump = moon.p
local test
do
  local _class_0
  local _base_0 = { }
  _base_0.__index = _base_0
  _class_0 = setmetatable({
    __init = function() end,
    __base = _base_0,
    __name = "test"
  }, {
    __index = _base_0,
    __call = function(cls, ...)
      local _self_0 = setmetatable({}, _base_0)
      cls.__init(_self_0, ...)
      return _self_0
    end
  })
  _base_0.__class = _class_0
  test = _class_0
end
test.__base.__properties = {
  ["p0"] = "test"
}
describe("classProperties", function()
  return it("get class properties", function()
    local testClassPrps = PO.classProperties(test)
    return assert.are.equal(testClassPrps, test.__base.__properties)
  end)
end)
return describe("metaMethods", function()
  return it("get class metaMethods", function()
    local testClassMetas = PO.metaMethods(test)
    assert.are.equal(testClassMetas, test.__base.__metas)
    return callable(test)
  end)
end)
