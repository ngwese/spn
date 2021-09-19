local mod = require 'core/mods'

-- grab this mods name (as installed) for use later in building path(s)
local this_name = mod.this_name
local _client = nil

local post_startup = function()
  -- extend the compile module path and load spn module globally
  package.cpath = package.cpath .. ";" .. paths.code .. this_name .. "/lib/?.so"
  _client = require 'spn'
end

local _null_touch = function(t, x, y, z, state, note)

local script_pre = function()
  -- ensure the global spn table has the
  spn = {}
  spn.touch = _null_touch
end

local script_post = function()
end

mod.hook.register("system_post_startup", "spn", post_startup)
mod.hook.register("script_pre_init", "spn", script_pre)
mod.hook.register("script_post_cleanup", "spn", script_post)

--
-- menu: zone selection, parameter modification
--

local menu = {}

menu.key = function(n, z)
  if n == 2 and z == 1 then
    -- return to the mod selection menu
    mod.menu.exit()
  end
end

m.enc = function(n, d)
  -- TODO:
  mod.menu.redraw()
end

m.redraw = function()
  screen.clear()
  screen.move(64,40)
  screen.text_center("spn")
  screen.update()
end

mod.menu.register(this_name, menu)

--
-- api
--

local api = {}

return api
