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
        Out.ar(out, sig * OnePole.kr(amp, 0.1));
      })
    );

    voices.add(
      SynthDef(\haze_two, {
        arg out=0, hz=300, amp=0.5, mod=0.5;
        var sig, tone, ampFinal, noiseAmpMap, noiseAmp, noise, noiseFreqs, noiseRingTimes;

        tone = SinOsc.ar(hz, mul: 0.3);

        amp = Clip.kr(amp);

        //noiseAmpMap = Env.new(levels: [0, 0, 0.6, 1], times: [0.1, 0.4, 0.5], curve: [\lin, \cub, \wel]);
        noiseAmpMap = Env.new(levels: [0, 0.6, 1], times: [0.6, 0.4], curve: [1, -2]);
        noiseAmp = IEnvGen.kr(noiseAmpMap, amp);

        noiseFreqs = [hz, hz * 0.5];
        noiseRingTimes = [1.4, 1];
        noise = DynKlank.ar(`[noiseFreqs, nil, noiseRingTimes], PinkNoise.ar(0.007));
        //noise = DynKlank.ar(`[[hz], nil, [1]], PinkNoise.ar(0.007));

        ampFinal = OnePole.kr(amp, 0.1);

        sig = XFade2.ar(tone * ampFinal, noise, (noiseAmp * 2) -1);
        sig = Mix.ar([sig, noise * Lag3.kr(ampFinal, 1)]);

        Out.ar(out, sig);
      })
    );

    voices.add(
      SynthDef(\haze_three, {
        arg out=0, hz=300, amp=0.5, mod=0.5;
        var sig, tone, toneA, toneB, ampFinal, noiseAmpMap, noiseAmp, noise, noiseFreqs, noiseRingTimes;

        toneA = SinOsc.ar(hz, mul: 0.3);
        toneB = PMOsc.ar(hz * 0.5, hz * (1.5 + PinkNoise.kr(0.007)), 3 * OnePole.kr(PinkNoise.kr(2), 0.8), mul: 0.05);
        tone = XFade2.ar(toneA, toneB, Clip.kr((mod * 2) - 1.5, -1, 1));

        amp = Clip.kr(amp);

        //noiseAmpMap = Env.new(levels: [0, 0, 0.6, 1], times: [0.1, 0.4, 0.5], curve: [\lin, \cub, \wel]);
        noiseAmpMap = Env.new(levels: [0, 0.6, 1], times: [0.6, 0.4], curve: [1, -2]);
        noiseAmp = IEnvGen.kr(noiseAmpMap, amp);

        noiseFreqs = [hz, hz * 0.5];
        noiseRingTimes = [1.4, 1];
        noise = Saw.ar((hz * 0.25) + PinkNoise.kr(8), 0.0005) + PinkNoise.ar(0.003);
        noise = DynKlank.ar(`[noiseFreqs, nil, noiseRingTimes], noise);
        //noise = DynKlank.ar(`[[hz], nil, [1]], PinkNoise.ar(0.007));

        ampFinal = OnePole.kr(amp, 0.1);

        sig = XFade2.ar(noise * ampFinal, tone * 0.6, (noiseAmp * 2) -1);
        //sig = Mix.ar([sig, noise * Lag3.kr(ampFinal, 1)]);

        Out.ar(out, sig);
      })
    );

    voices.add(
      SynthDef(\haze_four, {
        arg out=0, hz=300, amp=0.5, mod=0.5;
        var sig, tone, ampFinal, noiseAmpMap, noiseAmp, noise, noiseFreqs, noiseRingTimes;

        tone = SinOsc.ar(hz, mul: 0.3);

        amp = Clip.kr(amp);

        //noiseAmpMap = Env.new(levels: [0, 0, 0.6, 1], times: [0.1, 0.4, 0.5], curve: [\lin, \cub, \wel]);
        noiseAmpMap = Env.new(levels: [0, 0.6, 1], times: [0.6, 0.4], curve: [1, -2]);
        noiseAmp = IEnvGen.kr(noiseAmpMap, amp);

        noiseFreqs = [hz, hz * 0.5];
        noiseRingTimes = [1.4, 1];
        noise = Saw.ar(hz + PinkNoise.kr(8), 0.0002) + PinkNoise.ar(0.001);
        noise = DynKlank.ar(`[noiseFreqs, nil, noiseRingTimes], noise);
        //noise = DynKlank.ar(`[[hz], nil, [1]], PinkNoise.ar(0.007));

        ampFinal = OnePole.kr(amp, 0.1);

        sig = XFade2.ar(tone * ampFinal, noise, (noiseAmp * 2) -1);
        //sig = Mix.ar([sig, noise * Lag3.kr(ampFinal, 1)]);

        Out.ar(out, sig);
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