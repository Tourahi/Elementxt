local strict
do
  strict = { }
  strict.defined = { }
  strict.global = function(t)
    for k, v in pairs(t) do
      strict.defined[k] = true
      rawset(_G, k, v)
    end
  end
  strict.__newindex = function(t, k, v)
    return error("Cannot set undefined variable: " .. k, 2)
  end
  strict.__index = function(t, k)
    if strict.defined[k] == nil then
      return error("Cannot get undefined variable: " .. k, 2)
    end
  end
end
return setmetatable(_G, strict)
