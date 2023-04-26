// For benjiaomodular MiniVerb v2.x
// Based on reverbsc by Stephen Hensley

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;
float sample_rate;
static int LED_PIN = 23;

ReverbSc DSY_SDRAM_BSS verb;
static Jitter jitter;

float level, feedback, cv, cv_offset, mod;

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
    out[0][i] = dryIn + verbOut * ((1 - mod) + (jitter_out * mod)) * level;

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

  cv = 1-CtrlVal(A4); // pin 26
  cv_offset = CtrlVal(A2) * 2.f - 1.f; // pin 24
  mod = constrain(cv + cv_offset, 0.f, 1.f);

  analogWrite(LED_PIN, constrain(mod * 255.f, 0, 255));

  verb.SetFeedback(0.8 + 0.2 * feedback);
  verb.SetLpFreq(15000.0f);
}
