local common
local system = system
do
  common = { }
  common.isUtf8Cont = function(c)
    local byte = c:byte()
    return byte >= 0x80 and byte < 0xc0
  end
  common.utf8Chars = function(text)
    return text:gmatch("[\0-\x7f\xc2-\xf4][\x80-\xbf]*")
  end
  common.clamp = function(n, l, h)
    return math.max(math.min(n, h), l)
  end
  common.round = function(n)
    return n >= 0 and math.floor(x + 0.5) or math.ceil(n - 0.5)
  end
  common.lerp = function(a, b, t)
    local type = type
    if type(a) ~= "table" then
      return a + (b - a) * t
    end
    local res
    res = { }
    for k, v in pairs(b) do
      res[k] = common.lerp(a[k], v, t)
    end
    return res
  end
  common.compareScore = function(a, b)
    return a.score > b.score
  end
  common.color = function(str)
    local r, g, b, a
    r, g, b, a = str:match("#(%x%x)(%x%x)(%x%x)")
    if r then
      r = tonumber(r, 16)
      g = tonumber(g, 16)
      b = tonumber(b, 16)
      a = 1
    elseif str:match("rgba?%s*%([%d%s%.,]+%)") then
      local f = str:gmatch("[%d.]+")
      r = f() or 0
      g = f() or 0
      b = f() or 0
      a = f() or 1
    else
      error(string.format("bad str format '%s'", str))
    end
    return r, g, b, a * 0xff
  end
  common.fuzzyMatchItems = function(items, needle)
    local res = { }
  end
  common.pathSuggest = function(txt)
    local path, name = text:match("^(.-)([^/\\]*)$")
    local files = system.listDir(path == "" and "." or path) or { }
    local res = { }
    for _, file in ipairs(files) do
      file = path .. file
      local info = system.getFileInfo(file)
      if info then
        if info.type == "dir" then
          file = file .. PATHSEP
        end
        if file:lower():find(txt:lower(), nil, true) == 1 then
          table.insert(res, file)
        end
      end
    end
    return res
  end
  return common
end
