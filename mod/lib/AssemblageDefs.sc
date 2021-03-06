AssemblageDefs {
  classvar <voices;
  classvar <shapers;

  *initClass {

    //
    // Voices
    //

    voices = List.new;

    voices.add(
      SynthDef(\assemblage_sine, {
        arg out=0, hz=300, amp=0.5, mod=0.5;
        var sig = SinOsc.ar(hz, mul: 0.3);
        Out.ar(out, sig * OnePole.kr(amp, 0.1));
      })
    );

    voices.add(
      SynthDef(\assemblage_haze, {
        arg out=0, hz=300, amp=0.5, mod=0.5, barrel1=0.0, barrel2=0.0, barrel3=1.0;
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
      SynthDef(\assemblage_chime, {
        arg out=0, hz=300, amp=0.5, mod=0.5, gate=0, barrel1=0.0, barrel2=0.0, barrel3=1.0;
        var sig, ampEnv, pulseFreq, pulseTrain;

        ampEnv = Env.adsr(releaseTime: LinLin.kr(mod, 0, 1, 2, 0.2));
        pulseFreq = LinLin.kr(barrel1, dstlo: barrel2, dsthi: barrel3 * 12) * LinLin.kr(mod, 0, 1, 0.5, 2);
        pulseTrain = LFPulse.kr(pulseFreq, iphase: LFNoise0.kr(10, mul: 0.5, add: 0.5), width: 0.2) * gate;
        pulseTrain = Clip.kr(Trig.kr(gate) + pulseTrain);

        sig = SinOsc.ar(hz, mul: 0.3) * Lag.kr(amp);
        sig = sig * EnvGen.kr(ampEnv, pulseTrain);
        Out.ar(out, sig);
      })
    );

    //
    // Shapers
    //

    shapers = List.new;

    shapers.add(
      SynthDef(\assemblage_place, {
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

AssemblageDefs.definitions
AssemblageDefs.voiceNames
*/