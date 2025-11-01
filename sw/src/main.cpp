#include <Arduino.h>
#include <I2CTinyBB.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include "iis328dq.h"

#define INT 2
#define BUZ 3
#define LED 4

#define IIS328DQ_ADDR 0x18
#define REG_WHO_AM_I 0x0f
#define REG_CTRL_REG1 0x20

#define ARR_SZ(x) (sizeof((x)) / sizeof(*(x)))

volatile int _play = 1;

void iis328dq_init()
{
	uint8_t d = 0;
	for (size_t i = 0; i < IIS328DQ_INIT_COUNT; ++i) {
		uint8_t tmp[2] = { iis328dq_shake_init[i].reg, iis328dq_shake_init[i].value };
		I2CWrite(IIS328DQ_ADDR, tmp, 2);
		I2CReadReg(IIS328DQ_ADDR, tmp[0], &d, 1);
	}
}

void setup() {
  uint8_t d;
  clock_prescale_set(clock_div_1);
  DDRB = (1 << PB4) | (1 << PB3) | (1 << PB1) | (1 << PB0);
  PORTB = 0;
  MCUCR = 0; // low level-triggered interrupt
  GIMSK |= (1 << INT0);  // Enable INT0 external interrupt
  I2CInit(0, 1, 10);
  sei();
  iis328dq_init();
}

// Interrupt Service Routine for INT0
ISR(INT0_vect) {}

void enterSleep() {
  uint8_t d = 0;
  MCUCR = 0;
  GIMSK |= (1 << INT0);  // Enable INT0 external interrupt
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Set deep sleep mode
  sleep_enable();  // Enable sleep bit
  sei();  // Enable global interrupts
  sleep_cpu();  // Go to sleep - execution stops here
  
  // Code resumes here after wake-up
  sleep_disable();  // Disable sleep bit
  GIMSK &= ~(1 << INT0);  // disable INT0 external interrupt
  I2CReadReg(IIS328DQ_ADDR, IIS328DQ_INT1_SRC, &d, 1);
  _play = 1;
}

void play()
{
	int countdown = 700;
	while (countdown > 0) {
		digitalWrite(LED, HIGH);
		delay(25);
		digitalWrite(LED, LOW);
		countdown *= 0.84;
		delay(countdown);
	}
	_play = 0;
}

void loop() {
	if (_play) {
		play();
	}
	enterSleep();
}
