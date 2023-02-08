function init()
  spn.handlers.control = function(...) print('ctl', ...) end
  spn.handlers.touch = function(...) print('touch', ...) end
  x = spn.client.zone.new({
    name = 'foo',
    type = 'xy',
    bounds = {0, 0, 3, 3},
    ctrl1 = 12,
    ctrl2 = 13
  })
  y = spn.client.zone.new({
    name = 'bar',
    type = 'xy',
    bounds = {0, 3, 3, 2},
    ctrl1 = 14,
    ctrl2 = 15,
  })
  notes = spn.client.zone.new({
    name = 'baz',
    type = 'note_row',
    bounds = {3, 0, 27, 1},
    start_note = 57,
  })
  spn.client.start()
  spn.client.set_zones({x, y, notes})
  spn.client.set_property('data_rate', 10)
end

function cleanup()
  spn.client.stop()
end