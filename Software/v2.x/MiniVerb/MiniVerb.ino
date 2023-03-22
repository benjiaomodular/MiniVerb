// For benjiaomodular MiniVerb v2.x
// Based on reverbsc by Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;
float sample_rate;

ReverbSc DSY_SDRAM_BSS verb;
static Jitter jitter;

float level, feedback, mod, cv1, jitterMixKnob;

float CtrlVal(uint8_t pin) {
  analogReadResolution(16);
  return (analogRead(pin)) / 65535.f;
}

void MyCallback(float **in, float **out, size_t size) {
  float dryIn, verbOut;
  float jitter_out;

  for (size_t i = 0; i < size; i++) {
    dryIn = in[0][i];
    jitter_out = jitter.Process();
    
    verb.Process(dryIn, 0, &verbOut, 0);
    out[0][i] = dryIn + verbOut * ((1 - cv1) + (jitter_out * cv1)) * level;

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

  level = 0.1f;
  DAISY.begin(MyCallback);
}

void loop() {
  level = CtrlVal(A0); // pin 22
  feedback = CtrlVal(A1); // pin 23
  mod = CtrlVal(A2); // pin 24
  cv1 = 1-CtrlVal(A4); // pin 26
 
  verb.SetFeedback(0.8 + 0.2 * feedback);
  verb.SetLpFreq(mod * 20000.0f);
}
