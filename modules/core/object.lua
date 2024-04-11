local object
do
  local _class_0
  local _base_0 = {
    hasMinxins = function(self)
      return rawget(self.__class.__parent, "mixinsClass")
    end,
    getMixinsClass = function(obj)
      local parent = obj.__class.__parent
      assert((parent ~= nil), "The object does not have a parent class.")
      if rawget(parent, "mixinsClass") then
        return parent, false
      end
      local mixinsClass
      do
        local _class_1
        local _parent_0 = parent
        local _base_1 = { }
        _base_1.__index = _base_1
        setmetatable(_base_1, _parent_0.__base)
        _class_1 = setmetatable({
          __init = function(self, ...)
            return _class_1.__parent.__init(self, ...)
          end,
          __base = _base_1,
          __name = "mixinsClass",
          __parent = _parent_0
        }, {
          __index = function(cls, name)
            local val = rawget(_base_1, name)
            if val == nil then
              local parent = rawget(cls, "__parent")
              if parent then
                return parent[name]
              end
            else
              return val
            end
          end,
          __call = function(cls, ...)
            local _self_0 = setmetatable({}, _base_1)
            cls.__init(_self_0, ...)
            return _self_0
          end
        })
        _base_1.__class = _class_1
        local self = _class_1
        self.__name = tostring(obj.__name) .. "Mixins"
        self.mixinsClass = true
        if _parent_0.__inherited then
          _parent_0.__inherited(_parent_0, _class_1)
        end
        mixinsClass = _class_1
      end
      obj.__class.__parent = mixinsClass
      setmetatable(obj.__class.__base, mixinsClass.__class.__base)
      return mixinsClass, true
    end,
    include = function(self, otherClass)
      local otherClassName
      if type(otherClass) == "string" then
        otherClass, otherClassName = assert(require(otherClass)), otherClass
      end
      assert((otherClass.__class ~= object) and (otherClass.__class.__parent ~= object), "Object is including a class that is or extends Object. An included class should be a plain class and not another object.")
      local mixinsClass = self:getMixinsClass()
      if otherClass.__class.__parent then
        self:include(otherClass.__class.__parent)
      end
      assert(otherClass.__class.__base ~= nil, "Expecting a class when trying to include " .. tostring(otherClassName or otherClass) .. " into " .. tostring(self.__name))
      for k, v in pairs(otherClass.__class.__base) do
        local _continue_0 = false
        repeat
          if k:match("^__") then
            _continue_0 = true
            break
          end
          mixinsClass.__base[k] = v
          _continue_0 = true
        until true
        if not _continue_0 then
          break
        end
      end
      return true
    end
  }
  _base_0.__index = _base_0
  _class_0 = setmetatable({
    __init = function() end,
    __base = _base_0,
    __name = "object"
  }, {
    __index = _base_0,
    __call = function(cls, ...)
      local _self_0 = setmetatable({}, _base_0)
      cls.__init(_self_0, ...)
      return _self_0
    end
  })
  _base_0.__class = _class_0
  object = _class_0
  return _class_0
end
