local strict


with strict = {}

  .defined = {}

  .global = (t) ->
    for k, v in pairs t
      strict.defined[k] = true
      rawset _G, k, v
  
  -- So we dont look up/set values in __nxtindex when they dont exist in __index
  .__newindex = (t, k, v) ->
    error "Cannot set undefined variable: " .. k, 2

  .__index = (t, k) ->
    if strict.defined[k] == nil
      error "Cannot get undefined variable: " .. k, 2

  
setmetatable _G, strict