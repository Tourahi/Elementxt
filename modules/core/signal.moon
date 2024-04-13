
local signal

handlers = {}
all = {}

insert = table.insert
abort = {}

with signal = {}

  .register = (name, options = {}) ->
    error "Missing field 'desc'", 2 unless options.desc
    all[name] = option

  .unregister = (name) ->
    all[name] = nil
    handlers[name] = nil

  .getHandlers = (name) ->
    handlers[name] = handlers[name] or {}
    handlers[name]


  
  .connect = (name, handler, index) ->
    error "Unknown signal '#{name}'", 2 unless all[name]

    list = getHandlers name
    if not index or index > #list + 1 then index = #list + 1
    else if index < 1 then index = 1
    insert list, index, handler

  .disconnect = (name, handler) ->
    handlers[name] = [h for h in *handlers_for name when h != handler]

  .emit = (name, params, illegal) ->
    error "Unknown signal '#{name}'", 2 unless all[name]
    error "emit can only be called with two parameters", 2 if illegal
    error "expected table as second parameter", 2 if params and type(params) != 'table'

    for handler in *getHandlers name
      cr = coroutine.create (...) -> handler ...
      stat, ret = coroutine.resume cr, params

    if stat
      if ret == abort and coroutine.status(cr) == 'dead'
        return abort
      else
        -- TODO LOG ERR
        error "TODO SIGNAL ERR"
    false
