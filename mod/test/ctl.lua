function init()
  spn.handlers.control = function(...) print('ctl', ...) end
  x = spn.client.zone.new({
    name = 'foo',
    type = 'xy',
    bounds = {0, 0, 3, 3},
    ctrl1 = 12,
    ctrl2 = 13
  })
  spn.client.start()
  spn.client.set_zones({x})
  spn.client.set_property('data_rate', 10)
end

function cleanup()
  spn.client.stop()
end