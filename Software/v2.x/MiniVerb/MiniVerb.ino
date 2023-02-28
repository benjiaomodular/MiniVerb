// Based on reverbsc by Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;
float sample_rate;

ReverbSc DSY_SDRAM_BSS verb;

float level, feedback, mod, cv1;

float CtrlVal(uint8_t pin) {
  analogReadResolution(16);
  return (analogRead(pin)) / 65535.f;
}

void MyCallback(float **in, float **out, size_t size) {
  float dryIn, verbOut;

  for (size_t i = 0; i < size; i++) {
    dryIn = in[0][i];
    verb.Process(dryIn, 0, &verbOut, 0);
    out[0][i] = dryIn + verbOut * level;
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

  level = 0.1f;
  DAISY.begin(MyCallback);
}

void loop() {
  level = CtrlVal(A0); // pin 22
  feedback = CtrlVal(A1); // pin 23
  mod = CtrlVal(A2); // pin 24
//  cv1 = CtrlVal(A3); // pin 25

//  level = 1 - cv1; // pin 22
  verb.SetFeedback(0.8 + 0.2 * feedback);
  verb.SetLpFreq(mod * 20000.0f);
}
