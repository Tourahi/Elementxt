PO = assert require "propertyObj"
moon = assert require "moon"
dump = moon.p


class test

test.__base.__properties = {
  "p0": "test"
}

describe "classProperties", ->
  it "get class properties", ->
    testClassPrps = PO.classProperties test
    assert.are.equal testClassPrps, test.__base.__properties

describe "metaMethods", ->
  it "get class metaMethods", ->
    testClassMetas = PO.metaMethods test
    assert.are.equal testClassMetas, test.__base.__metas
    callable test