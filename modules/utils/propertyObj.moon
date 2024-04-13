
callable = (obj) ->
  return type(obj) == 'function' or getmetatable(obj) and getmetatable(obj).__call

-- GETTERS

classProperties = (cls) ->
  cls.__base.__properties = {} if cls.__base.__properties == nil
  cls.__base.__properties

metaMethods = (cls) ->
  cls.__base.__metas = {} if cls.__base.__metas == nil
  cls.__base.__metas

delegate = (target, key) ->
  return nil unless target
  val = target[key]
  return val unless callable val
  return (self, ...) ->
    val target, ...





{
  classProperties: classProperties
  metaMethods: metaMethods
}