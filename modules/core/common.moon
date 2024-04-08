local common

system = system

with common = {}
  .isUtf8Cont = (c) ->
    byte = c\byte!
    byte >= 0x80 and byte < 0xc0

  .utf8Chars = (text) ->
    text\gmatch "[\0-\x7f\xc2-\xf4][\x80-\xbf]*"

  .clamp = (n, l, h) ->
    math.max math.min(n, h), l

  .round = (n) ->
    n >= 0 and math.floor(x + 0.5) or math.ceil(n - 0.5)

  .lerp = (a, b, t) ->
    type = type
    if type(a) ~= "table"
      return a + (b - a) * t
    local res
    res = {}
    for k, v in pairs b
      res[k] = common.lerp a[k], v, t
    res

  .compareScore = (a, b) ->
    a.score > b.score

  .color = (str) ->
    local r, g, b, a
    r, g, b, a = str\match "#(%x%x)(%x%x)(%x%x)" -- FF0000
    
    if r
      r = tonumber r, 16
      g = tonumber g, 16
      b = tonumber b, 16
      a = 1 -- for now TODO
    elseif str\match "rgba?%s*%([%d%s%.,]+%)" -- rgba(x,x,x)
      f = str\gmatch "[%d.]+"
      r = f! or 0
      g = f! or 0
      b = f! or 0
      a = f! or 1
    else
      error string.format "bad str format '%s'", str
      
    r, g, b, a * 0xff -- convert a to 255 val

    
  .fuzzyMatchItems = (items, needle) ->
    res = {}
    -- TODO

  .pathSuggest = (txt) ->
    path, name = text\match "^(.-)([^/\\]*)$"
    files = system.listDir(path == "" and "." or path) or {}
    res = {}

    for _, file in ipairs files
      file = path .. file
      info = system.getFileInfo file
      if info
        if info.type == "dir"
          file ..= PATHSEP -- Global from main.cpp
        
        if file\lower!\find(txt\lower!, nil, true) == 1
          table.insert res, file

    res

