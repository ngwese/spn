mu = require('musicutil')

engine.name = 'Haze'

touches_max = nil
touches_hist = 16
touches = {}
dirty = false
redraw_clock = nil

local actions = {}

-- soundplane touch callback
-- @tparam number t touch/voice number
-- @tparam number x horizontal position within the zone [0-1]
-- @tparam number y vertical position within the zone [0-1]
-- @tparam number z pressure [0-1]
-- @tparam number state, 1 = start,  2 = update, 3 = release
-- @tparam number note note number for x within zone, fractional
--    component represents semitone pitch bend
function actions.touch(t, x, y, z, note, state)
  -- print(t, x, y, z, note, state)

  engine.touch(t - 1, mu.note_num_to_freq(note), z, y)  
  
  local this = touches[t]
  -- transform the touch into screen space
  this.x = util.linlin(0, 1, 0, 128, x)
  this.y = util.linlin(0, 1, 0, 64, 1 - y)
  this.z = util.linlin(0, 1, 3, 21, z)
  this.state = state
  this.note = note
  dirty = true
end

function redraw()
  if not dirty then return end

  screen.clear()
  for i=1,touches_max do
    local touch = touches[i]
    local x, y, z = touch.x, touch.y, touch.z
    local state = touch.state
    if (state == 2 or state == 1) and x and y and z then
      screen.level(15)
      screen.circle(x, y, z)
      screen.move(x, y+2)
      screen.text_center(i)
      screen.stroke()
    end
  end
  screen.update()
  dirty = false
end


function init()
  spn.add_params(false)
  spn.handlers = actions
  spn.client.start()

  touches_max = spn.client.get_property("max_touches")
  for i=1,touches_max do
    touches[i] = {}
  end

  screen.aa(2)

  redraw_clock = clock.run(function()
    local interval = 1/15
    while true do
      redraw()
      clock.sleep(interval)
    end
  end)
end

function cleanup()
  if redraw_clock then
    clock.cancel(redraw_clock)
  end
end

