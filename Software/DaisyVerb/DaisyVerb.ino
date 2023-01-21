// Title: reverbsc
// Description: Applies reverb to input signal
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;
float sample_rate;

ReverbSc DSY_SDRAM_BSS verb;
PitchShifter DSY_SDRAM_BSS ps;

static Jitter jitter;
static Bitcrush bitcrush;

Overdrive drive;

static float dryLevel, wetLevel, send;
float jitterMixKnob;
float srKnob;


float CtrlVal(uint8_t pin) {
  analogReadResolution(16);
  return (analogRead(pin)) / 65535.f;
}

void MyCallback(float **in, float **out, size_t size) {
  float dryL, dryR, verbL, verbR;
  float crushed;
  float jitter_out;

  for (size_t i = 0; i < size; i++) {
    dryL = in[0][i];
    dryR = in[1][i];

    jitter_out = jitter.Process();
    verb.Process(dryL, dryR, &verbL, &verbR);

    out[0][i] = (dryL * dryLevel) + verbL * ((1 - jitterMixKnob) + (jitter_out * jitterMixKnob)) * wetLevel;
    out[1][i] = (dryR * dryLevel) + verbR;
  }
}

void setup() {

  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  // Initialize reverb
  verb.Init(sample_rate);
  verb.SetFeedback(0.95f);
  verb.SetLpFreq(18000.0f);

  jitter.Init(sample_rate);
  jitter.SetAmp(1);
  jitter.SetCpsMin(1);
  jitter.SetCpsMax(25);

  wetLevel = 0.1f;
  DAISY.begin(MyCallback);
}

void loop() {
  dryLevel = CtrlVal(A0); // pin 22
  wetLevel = CtrlVal(A1); // pin 23
  verb.SetFeedback(0.8f + CtrlVal(A2) * .199f); // pin 24
  jitterMixKnob = CtrlVal(A3); // pin 25
  verb.SetLpFreq(CtrlVal(A4) * 20000.0f); // pin 26

}
