engine.name = 'None'

touches_hist = 16
touches_off = {}
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
  if t > spn.max_touches then return end

  local off = (touches_off[t] % touches_hist) + 1
  local this = touches[t][off]
  -- transform the touch into screen space
  this.x = util.linlin(0, 1, 0, 128, x)
  this.y = util.linlin(0, 1, 0, 64, 1 - y)
  this.z = z_radius(z)
  this.state = state
  this.note = note
  -- print('----------------')
  -- tab.print(this)
  touches_off[t] = off
  dirty = true
end

function actions.frame_begin(ts)
  -- print("begin", ts)
end

function actions.frame_end()
  -- print("end")
end


function z_radius(z)
  return util.linlin(0, 1, 3, 21, z)
end

function redraw()
  if not dirty then return end

  screen.clear()
  for i=1,touches_max do
    local offset = touches_off[i]
    local touch = touches[i][offset]
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
  spn.client.start()
  spn.add_params(false)
  spn.set_max_touches(8)
  spn.handlers = actions

  for i=1,spn.max_touches do
    touches_off[i] = 1
    touches[i] = {}
    for h=1,touches_hist do
      -- pre-allocate a tables in history
      touches[i][h] = {}
    end
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

