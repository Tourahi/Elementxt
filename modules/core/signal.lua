local signal
local handlers = { }
local all = { }
local insert = table.insert
local abort = { }
do
  signal = { }
  signal.register = function(name, options)
    if options == nil then
      options = { }
    end
    if not (options.desc) then
      error("Missing field 'desc'", 2)
    end
    all[name] = option
  end
  signal.unregister = function(name)
    all[name] = nil
    handlers[name] = nil
  end
  signal.getHandlers = function(name)
    handlers[name] = handlers[name] or { }
    return handlers[name]
  end
  signal.connect = function(name, handler, index)
    if not (all[name]) then
      error("Unknown signal '" .. tostring(name) .. "'", 2)
    end
    local list = getHandlers(name)
    if not index or index > #list + 1 then
      index = #list + 1
    else
      if index < 1 then
        index = 1
      end
    end
    return insert(list, index, handler)
  end
  signal.disconnect = function(name, handler)
    do
      local _accum_0 = { }
      local _len_0 = 1
      local _list_0 = handlers_for(name)
      for _index_0 = 1, #_list_0 do
        local h = _list_0[_index_0]
        if h ~= handler then
          _accum_0[_len_0] = h
          _len_0 = _len_0 + 1
        end
      end
      handlers[name] = _accum_0
    end
  end
  signal.emit = function(name, params, illegal)
    if not (all[name]) then
      error("Unknown signal '" .. tostring(name) .. "'", 2)
    end
    if illegal then
      error("emit can only be called with two parameters", 2)
    end
    if params and type(params) ~= 'table' then
      error("expected table as second parameter", 2)
    end
    local _list_0 = getHandlers(name)
    for _index_0 = 1, #_list_0 do
      local handler = _list_0[_index_0]
      local cr = coroutine.create(function(...)
        return handler(...)
      end)
      local stat, ret = coroutine.resume(cr, params)
    end
    if stat then
      if ret == abort and coroutine.status(cr) == 'dead' then
        return abort
      else
        error("TODO SIGNAL ERR")
      end
    end
    return false
  end
  return signal
end
