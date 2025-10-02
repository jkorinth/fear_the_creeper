#include <Arduino.h>
#include <TinyWireM.h>
#include <avr/power.h>
#include "lut.h"

const int LED = 4;
const int BUZ = 3;

#define PIEZO_PIN  BUZ
#define PIEZO_PORT PORTB
#define PIEZO_BIT (1 << PIEZO_PIN)

void setup() {
  clock_prescale_set(clock_div_1);
  pinMode(LED, OUTPUT);
  pinMode(BUZ, OUTPUT);
}

void hiss() {
	tone(BUZ, random(5000, 19999));
	delay(random(1, 10));
}

void bang() {
  int startFreq = 2000;  // Start high (the "crack")
  int endFreq = 200;     // End low (the "thud")
  int duration = 250;    // Total time of the bang in ms
  int stepDelay = 5;     // Delay between frequency steps (ms)

  for (int freq = startFreq; freq >= endFreq; freq -= (startFreq - endFreq) / (duration / stepDelay)) {
    tone(PIEZO_PIN, freq);
    delay(stepDelay);
  }
  noTone(PIEZO_PIN);
}


void loop() {
	static int i = 0;
	digitalWrite(LED, HIGH);
	delay(10);
	if (i < 100) {
		digitalWrite(LED, LOW);
		delayMicroseconds(pgm_read_word(&LUT[i++]));
	}
	return;
	static int state = 0;
	static int w = 0;
	if (!state) {
		hiss();
		w += 1;
		//delay(250);
		if (w > 1000) {
			state++;
		}
	} else {
		bang();
		bang();
		bang();
		state = 0;
		w = 0;
		delay(1000);
	}
}
