HazeDefs {
  classvar <voices;
  classvar <shapers;

  *initClass {

    //
    // Voices
    //

    voices = List.new;

    voices.add(
      SynthDef(\haze_sine, {
        arg out=0, hz=300, amp=0.5, mod=0.5;
        var sig = SinOsc.ar(hz, mul: 0.3);
        Out.ar(out, sig * amp);
      })
    );


    //
    // Shapers
    //

    shapers = List.new;

    shapers.add(
      SynthDef(\haze_place, {
        arg out=0, in=0, amp=0.2;
        // TODO: panning
        var sig = In.ar(in) * amp;
        Out.ar(out, sig!2);
      })
    );
  }

  *definitions {
    ^voices ++ shapers;
  }

  *collectNames { arg defs;
    ^defs.collectAs({arg d; d.name}, List);
  }

  *voiceNames {
    ^this.collectNames(voices);
  }
}

/*

HazeDefs.definitions
HazeDefs.voiceNames
*/