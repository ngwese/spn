local mod = require('core/mods')

-- grab this mods name (as installed) for use later in building path(s)
local this_name = mod.this_name

local spn_params = require(this_name .. '/lib/param')

local spn_null_handlers = function()
  return {
    frame_begin = function(...) end,
    frame_end = function(...) end,
    touch = function(...) end,
    control = function(...) end,
  }
end

local spn_set_max_touches = function(n)
  spn.client.set_property('max_touches', n)
  spn.max_touches = n
end

local spn_init = function()
  -- ensure the global spn table has the goods
  spn = {
    client = require '_spn',
    handlers = spn_null_handlers(),
    set_max_touches = spn_set_max_touches,
    add_touch_params = spn_params.add_touch_params,
    add_voice_params = spn_params.add_voice_params,
  }
end

local spn_clean = function()
  if spn.client.is_running() then
    spn.client.stop()
  end
end


--
-- udev rules
--
-- non-class compliant usb devices are ignored by udev by default. in order for
-- the soundplace hardware to be detected it must be allow listed. normally the
-- udev rules installation is handled by the soundplacelib debian packaging but
-- this driver is entirely self contained thus it needs to handle installation
-- directly.
--

local rules_file = "59-soundplane.rules"
local rules_dst = "/etc/udev/rules.d/" .. rules_file

local spn_udev_rules_installed = function()
  local f = io.open(rules_dst, "r")
  if f ~= nil then
    io.close(f)
    return true
  end

  return false
end

local spn_install_udev_rules = function()
  local rules_src = paths.code .. this_name .. "/lib/support/" .. rules_file

  print("spn: installing udev rules to allow device detection")
  local cmd_cp = "sudo cp -v " .. rules_src .. " " .. rules_dst
  print("   >> '" .. cmd_cp .. "'")
  os.execute(cmd_cp)

  local cmd_reload = "sudo udevadm control --reload"
  print("   >> '" .. cmd_reload .. "'")
  os.execute(cmd_reload)
  print("spn: udev rule installation complete")
end

local post_startup = function()
  -- extend the compile module path and load spn module globally
  package.cpath = package.cpath .. ";" .. paths.code .. this_name .. "/lib/?.so"
  if not spn_udev_rules_installed() then
    spn_install_udev_rules()
  end
  spn_init()
end

mod.hook.register("system_post_startup", "spn_startup", post_startup)
mod.hook.register("script_pre_init", "spn_init", spn_init)
mod.hook.register("script_post_cleanup", "spn_cleanup", spn_clean)

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

local api = {
  install_udev_rules = spn_install_udev_rules,
}

return api
