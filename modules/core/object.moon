
class object

  -- @local
  hasMinxins: =>
    rawget(@__class.__parent, "mixinsClass")

  --- get the class where mixins can be inserted.
  -- @treturn table {mixinTable, boolean(If mixin class was created)}
  getMixinsClass: (obj) ->
    parent = obj.__class.__parent -- The parent class

    assert (parent != nil),
      "The object does not have a parent class."

    if rawget(parent, "mixinsClass") then return parent, false

    mixinsClass = class extends parent
      @__name: "#{obj.__name}Mixins"
      @mixinsClass: true

    obj.__class.__parent = mixinsClass
    setmetatable obj.__class.__base, mixinsClass.__class.__base

    mixinsClass, true

  include: (otherClass) =>
    otherClass, otherClassName = if type(otherClass) == "string"
      assert(require(otherClass)), otherClass

    assert (otherClass.__class != object) and (otherClass.__class.__parent != object),
      "Object is including a class that is or extends Object. An included class should be a plain class and not another object."

    mixinsClass = @getMixinsClass!

    if otherClass.__class.__parent then @include otherClass.__class.__parent

    assert otherClass.__class.__base != nil, "Expecting a class when trying to include #{otherClassName or otherClass} into #{@__name}"

    for k, v in pairs otherClass.__class.__base
      continue if k\match("^__")
      mixinsClass.__base[k] = v

    true