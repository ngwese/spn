Assemblage {
  var <server;

  var <parentGroup;
  var <outBus;

  var <defaultTable;
  var <randomTable;

  var <globalDefaults;
  var <globalControls;

  var <modGroup;
  var <voicesGroup;
  var <fxGroup;

  var <voiceType;

  var <voices;
  var <voiceStarted;

  // Create and initialize a new Assemblage instance on the given server
  *new { arg server, group, out=0, voiceCount=8;
    ^super.new.init(server, group, out, voiceCount);
  }

  // Initialize class and define server side resources
  init { arg argServer, argGroup, argOut, argVoiceCount;
    var serverInitialized = Condition.new;

    server = argServer;
    parentGroup = if(argGroup.isNil, {server}, {argGroup});
    outBus = argOut;

    modGroup = Group.new(parentGroup);
    voicesGroup = Group.after(modGroup);
    fxGroup = Group.after(voicesGroup);

    voices = Array.new;

    Post << "Assemblage.init: server = " << server << ", xg = " << parentGroup << ", out = " << outBus << "\n";

    fork {
      Post << "Assemblage.init: sending defs...\n";

      AssemblageDefs.definitions.do({ arg d;
        postln(" +" + d.name);
        d.send(server);
      });

      server.sync;

      serverInitialized.test = true;
      serverInitialized.signal;

      Post << "Assemblage.init: server synced\n";
    };

    // Control busses for global parameters which drive all voices (non per-note parameters)
    globalDefaults = IdentityDictionary.with(
    //   \cutoff -> 1200,
    //   \resonance -> 0.0,
    );

    globalControls = globalDefaults.keys.collectAs({ arg n;
      n -> Bus.control(server, 1)
    }, IdentityDictionary);

    this.setControlDefaults;

    fork {
      // wait for defs and default buffers to land
      serverInitialized.wait;

      Post << "Assemblage.init: setting default configuration...\n";
      voiceType = \assemblage_haze;

      Post << "Assemblage.init: allocating voices...\n";
      this.allocVoices(argVoiceCount);

      Post << "Assemblage.init: done\n";
    };
  }

  free {
    voices.do({_.free});
    globalControls.values.do({_.free});
    modGroup.free;
    voicesGroup.free;
    fxGroup.free;
  }

  // Sets all global control busses to their default values
  setControlDefaults {
    globalControls.keys.do({ arg n;
      globalControls[n].setSynchronous(globalDefaults[n]);
    })
  }

  setControl { arg name, value;
    globalControls[name].set(value);
  }

  allocVoices {
    arg n;

    if(voices.notNil, {
      voices.do({arg v; v.free });
    });

    voices = n.collectAs({ AssemblageVoice.new(
      this.server,
      this.voicesGroup,
      voiceType: this.voiceType,
      out: this.outBus,
    ) }, Array);
  }

  voiceCount {
    ^voices.size;
  }

  touch {
    arg voice=0, hz=300, amp=0.2, mod=0.5;
    voices[voice].touch(hz, amp, mod);
  }

  stop {
    arg voice=0;
    voices[voice].stop;
  }

  stopAll {
    voices.do({arg v; v.stop});
  }

}

AssemblageVoice {
  var <voiceSynth;
  var <voiceType;
  var <voiceGroup;

  var <voiceBus;

  var <pitchBus;   // x
  var <modBus;     // y
  var <ampBus;     // z

  var <shapeSynth;
  var <shapeType;
  var <shapeGroup;

  var <shapeAmpBus;

  *new { arg server, target, voiceType, out=0;
    ^super.new.init(server, target, voiceType, out);
  }

  init {
    arg server, target, voiceType, out;

    Post << "AssemblageVoice.init(" << server << ", " << target << ", " << out << ", "
    << voiceType << ")\n";

    // create a group to contain the synths for the voice
    voiceGroup = Group.new(target);
    voiceBus = Bus.audio(server, 1);

    // create a voice specific control busses
    pitchBus = Bus.control(server, 1);
    modBus = Bus.control(server, 1);
    ampBus = Bus.control(server, 1);

    // set controls to something reasonable so DynKlank doesn't blow up
    pitchBus.set(440);
    modBus.set(0.5);
    ampBus.set(0);

    // create a group to contain the synths to shape the voice and write to the output
    shapeGroup = Group.after(voiceGroup);
    shapeAmpBus = Bus.control(server, 1);

    // create osc synths (before mixer), mapping modulators,
    this.voiceType = voiceType;

    // scale voice ouput by amp env (after filter) and write to main bus
    shapeType = \assemblage_place;  // TODO: fixed for now
    shapeSynth = Synth.new(shapeType, [\out, out, \in, voiceBus], shapeGroup);
    shapeSynth.map(\amp, shapeAmpBus);
    shapeAmpBus.set(1);

    // map shared voice controls
    // modGroup.map(\gate, gateBus);
    // oscGroup.map(\hz, pitchBus);
  }

  voiceType_ { arg symbol;
    voiceType = symbol;
    if (voiceSynth.notNil) {
      voiceSynth.free;
      voiceSynth = nil;
    };
    if (voiceType.notNil) {
      voiceSynth = Synth.new(voiceType, [\out, voiceBus.index], voiceGroup);
      // FIXME: this isn't going to pick of the shared voice group mappings
      voiceSynth.map(\hz, pitchBus, \amp, ampBus, \mod, modBus);
    };
  }

  touch {
    arg hz, amp, mod;
    pitchBus.set(hz);
    modBus.set(mod);
    ampBus.set(amp);

    // Post << "  pitch: " << hz << "\n";
    // Post << "    amp: " << amp << "\n";
    // Post << "    mod: " << mod << "\n";
  }

  stop {
    ampBus.set(0);
  }

  level {
    arg amp;
    shapeAmpBus.set(amp);
  }


  free {
    voiceGroup.freeAll;
    voiceGroup.free;
    shapeGroup.freeAll;
    shapeGroup.free;
    voiceBus.free;
    pitchBus.free;
    ampBus.free;
    modBus.free;
  }

  setPitch { arg hz;
    pitchBus.set(hz);
  }

  setAmp { arg amp;
    ampBus.set(amp);
  }
}


/*

~processGroup = Crone.context.xg;
~f = Assemblage.new(s, ~processGroup, voiceCount: 2);

~f.voiceCount;
~f.free

~f.allocVoices(1);

~f.touch(0, 400, 0.2, 0.5);
~f.touch(0, 400, 0.5, 0.5);
~f.touch(0, 500, 0.7, 0.5);
~f.touch(0, 500, 0.5, 0.1);
~f.touch(0, 500, 0.5, 0.7);

~f.touch(0, 440, 1, 0.5);
~f.stop(0);

~f.voices[0]

*/