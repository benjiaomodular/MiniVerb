// For benjiaomodular MiniVerb v2.x

// -- References --
// Based on reverbsc and pitchshifter by Stephen Hensley
// https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/reverbsc/reverbsc.ino
// https://github.com/electro-smith/DaisyDuino/blob/master/examples/Seed/pitchshifter/pitchshifter.ino

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;
float sample_rate;
static int LED_PIN = 23;

ReverbSc DSY_SDRAM_BSS verb;
PitchShifter ps;

float level, feedback, cv, cv_offset, mod;

float CtrlVal(uint8_t pin) {
  analogReadResolution(16);
  return (analogRead(pin)) / 65535.f;
}

void MyCallback(float **in, float **out, size_t size) {
  float dryIn, verbOut;
  float shifted;

  for (size_t i = 0; i < size; i++) {
    dryIn = in[0][i];
    shifted = ps.Process(dryIn);
    
    verb.Process(dryIn +  shifted * mod, 0, &verbOut, 0);
    out[0][i] = dryIn + verbOut * level;

  }
}

void setup() {

  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();
  
  pinMode(LED_PIN, OUTPUT);

  // Initialize reverb
  verb.Init(sample_rate);
  verb.SetFeedback(0.95f);
  verb.SetLpFreq(18000.0f);

  ps.Init(sample_rate);
  ps.SetTransposition(12.0f);

  level = 0.1f;
  DAISY.begin(MyCallback);
}

void loop() {
  level = CtrlVal(A0); // pin 22
  feedback = CtrlVal(A1); // pin 23

  cv = 1-CtrlVal(A4); // pin 26
  cv_offset = CtrlVal(A2) * 2.f - 1.f; // pin 24
  mod = constrain(cv + cv_offset, 0.f, 1.f);

  analogWrite(LED_PIN, constrain(mod * 255.f, 0, 255));

  verb.SetFeedback(0.8 + 0.2 * feedback);
  verb.SetLpFreq(15000.0f);
}
