local mod = require 'core/mods'

-- grab this mods name (as installed) for use later in building path(s)
local this_name = mod.this_name

local spn_init = function()
  -- ensure the global spn table has the
  spn = {}
  spn.touch = function(t, x, y, z, state, note) end
  spn.client = require 'spn'
end

local post_startup = function()
  -- extend the compile module path and load spn module globally
  package.cpath = package.cpath .. ";" .. paths.code .. this_name .. "/lib/?.so"
  spn_init()
end

local script_post = function()
end

mod.hook.register("system_post_startup", "spn_startup", post_startup)
mod.hook.register("script_pre_init", "spn_init", spn_init)
mod.hook.register("script_post_cleanup", "spn_cleanup", script_post)

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

menu.enc = function(n, d)
  -- TODO:
  mod.menu.redraw()
end

menu.redraw = function()
  screen.clear()
  screen.move(64,40)
  screen.text_center("spn")
  screen.update()
end

menu.init = function() end -- on menu entry, ie, if you wanted to start timers
menu.deinit = function() end -- on menu exit

mod.menu.register(this_name, menu)

--
-- api
--

local api = {}

return api
