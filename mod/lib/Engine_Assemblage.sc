Engine_Assemblage : CroneEngine {
  var <kernel;

  *new { arg context, doneCallback;
		^super.new(context, doneCallback);
	}

  alloc {
    kernel = Assemblage.new(this.context.server, this.context.xg, this.context.out_b);

    context.server.sync;

    // voice, pitch (x), pressure (z), depth (y), x raw, gate
    this.addCommand(\touch, "iffffi", { arg msg;
      kernel.touch(msg[1], msg[2], msg[3], msg[4], msg[5], msg[6]);
    });

    // silence voice i
    this.addCommand(\stop, "i", { arg msg;
      kernel.stop(msg[1]);
    });

    this.addCommand(\stop_all, "", { arg msg;
      kernel.stopAll();
    });

    this.addCommand(\voices, "i", { arg msg;
      kernel.allocVoices(msg[1]);
    });

    this.addCommand(\barrel1, "f", { arg msg;
      kernel.setControl(\barrel1, msg[1]);
    });

    this.addCommand(\barrel2, "f", { arg msg;
      kernel.setControl(\barrel2, msg[1]);
    });

    this.addCommand(\barrel3, "f", { arg msg;
      kernel.setControl(\barrel3, msg[1]);
    });

  }

  free {
    kernel.free;
  }
}

/*




*/