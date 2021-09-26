local function add_params(group, name)
  local name = name or 'SPN'
  if group then
    params:add_group(name, 15)
  else
    params:add_separator(name)
  end

  params:add{type = 'number', id = 'spn_max_touches', name = 'max touches',
    min = 1, max = 16, default = 8,
    action = function(v)
      spn.client.set_property('max_touches', v)
    end
  }
  params:add{type = 'option', id = 'spn_quantize', name = 'quantize',
    options = {'off', 'on'},
    default = 1,
    action = function(v)
      spn.client.set_property('quantize', v - 1)
    end
  }
  params:add{type = 'option', id = 'spn_note_lock', name = 'note lock',
    options = {'off', 'on'},
    default = 1,
    action = function(v)
      spn.client.set_property('lock', v - 1)
    end
  }
  params:add{type = 'option', id = 'spn_rotate', name = 'rotate',
    options = {'off', 'on'},
    default = 1,
    action = function(v)
      spn.client.set_property('rotate', v - 1)
    end
  }
  params:add{type = 'number', id = 'spn_portamento', name = 'portamento',
    min = 0, max = 1000, default = 250,
    action = function(v)
      spn.client.set_property('snap', v)
    end
  }
  params:add{type = 'control', id = 'spn_vibrato', name = 'vibrato',
    controlspec = controlspec.new(0.0, 1.0, 'lin', 0, 0, ''),
    action = function(v)
      spn.client.set_property('vibrato', v)
    end
  }
  params:add{type = 'number', id = 'spn_transpose', name = 'transpose',
    min = -24, max = 24, default = 0,
    action = function(v)
      spn.client.set_property('transpose', v)
    end
  }
  params:add{type = 'number', id = 'spn_lopass_z', name = 'lopass z',
    min = 1, max = 250, default = 100,
    action = function(v)
      spn.client.set_property('lopass_z', v)
    end
  }
  params:add{type = 'control', id = 'spn_z_thresh', name = 'z thresh',
    controlspec = controlspec.new(0.0, 0.10, 'lin', 0, 0, ''),
    action = function(v)
      spn.client.set_property('z_thresh', v)
    end
  }
  params:add{type = 'control', id = 'spn_z_scale', name = 'z scale',
    controlspec = controlspec.new(0.25, 5, 'lin', 0, 0, ''),
    action = function(v)
      spn.client.set_property('z_scale', v)
    end
  }
  params:add{type = 'control', id = 'spn_z_curve', name = 'z curve',
    controlspec = controlspec.new(0.0, 1.0, 'lin', 0, 0, ''),
    action = function(v)
      spn.client.set_property('z_curve', v)
    end
  }
  params:add{type = 'control', id = 'spn_hysteresis', name = 'hysteresis',
    controlspec = controlspec.new(0.01, 1.0, 'lin', 0, 0, ''),
    action = function(v)
      spn.client.set_property('hysteresis', v)
    end
  }
  params:add{type = 'number', id = 'spn_data_rate', name = 'date rate',
    min = 1, max = 500, default = 100,
    action = function(v)
      spn.client.set_property('data_rate', v)
    end
  }
  params:add{type = 'option', id = 'spn_verbose', name = 'verbose',
    options = {'off', 'on'},
    default = 1,
    action = function(v)
      spn.client.set_property('verbose', v - 1)
    end
  }
  params:add{type = 'trigger', id = 'spn_calibrate', name = 'calibrate',
    action = function()
      print('TODO: trigger calibrate')
    end
  }
end

return {
  add_params = add_params,
}
