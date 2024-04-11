local callable
callable = function(obj)
  return type(obj) == 'function' or getmetatable(obj) and getmetatable(obj).__call
end
local classProperties
classProperties = function(cls)
  if cls.__base.__properties == nil then
    cls.__base.__properties = { }
  end
  return cls.__base.__properties
end
local metaMethods
metaMethods = function(cls)
  if cls.__base.__metas == nil then
    cls.__base.__metas = { }
  end
  return cls.__base.__metas
end
local delegate
delegate = function(target, key)
  if not (target) then
    return nil
  end
  local val = target[key]
  if not (callable(val)) then
    return val
  end
  return function(self, ...)
    return val(target, ...)
  end
end
return {
  classProperties = classProperties,
  metaMethods = metaMethods
}
