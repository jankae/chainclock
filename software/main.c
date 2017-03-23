#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define BLUE_HIGH()		(PORTD |= (1<<PD5))
#define BLUE_LOW()		(PORTD &= ~(1<<PD5))

#define PINK_HIGH()		(PORTB |= (1<<PB2))
#define PINK_LOW()		(PORTB &= ~(1<<PB2))

#define ORANGE_HIGH()		(PORTB |= (1<<PB3))
#define ORANGE_LOW()		(PORTB &= ~(1<<PB3))

#define YELLOW_HIGH()		(PORTB |= (1<<PB4))
#define YELLOW_LOW()		(PORTB &= ~(1<<PB4))

// counter for the motor steps
// 1019/15 steps per minute needed
// needed rotations*15 -> 1019 of this per minute
uint16_t neededRotations15;

uint8_t minuteParts;
uint8_t minuteFlag;

uint8_t stepCnt = 0;

uint8_t batteryLow = 0;

void waitms(uint16_t ms) {
	for (; ms > 0; ms--)
		_delay_ms(1);
}

void stopMotor(void) {
	ORANGE_LOW();
	YELLOW_LOW();
	PINK_LOW();
	BLUE_LOW();
}

void moveStep(void) {
	stepCnt += 1;
	if (stepCnt >= 8)
		stepCnt = 0;
	switch (stepCnt) {
	case 0:
		ORANGE_HIGH();
		YELLOW_LOW();
		PINK_LOW();
		BLUE_LOW();
		break;
	case 1:
		ORANGE_HIGH();
		YELLOW_HIGH();
		PINK_LOW();
		BLUE_LOW();
		break;
	case 2:
		ORANGE_LOW();
		YELLOW_HIGH();
		PINK_LOW();
		BLUE_LOW();
		break;
	case 3:
		ORANGE_LOW();
		YELLOW_HIGH();
		PINK_HIGH();
		BLUE_LOW();
		break;
	case 4:
		ORANGE_LOW();
		YELLOW_LOW();
		PINK_HIGH();
		BLUE_LOW();
		break;
	case 5:
		ORANGE_LOW();
		YELLOW_LOW();
		PINK_HIGH();
		BLUE_HIGH();
		break;
	case 6:
		ORANGE_LOW();
		YELLOW_LOW();
		PINK_LOW();
		BLUE_HIGH();
		break;
	case 7:
		ORANGE_HIGH();
		YELLOW_LOW();
		PINK_LOW();
		BLUE_HIGH();
		break;
	}
}

void moveOneMinute(void) {
	neededRotations15 += 1019;
	minuteParts = 0;
	minuteFlag = 0;
	uint8_t additionalRotations = neededRotations15 / 15;
	neededRotations15 -= additionalRotations * 15;
	// enable analog comparator for detecting low battery
	ACSR &= ~(1 << ACD);
	// select bandgap reference
	ACSR |= (1 << ACBG);
	// enable voltage divider
	PORTD &= ~(1 << PD4);
	while (additionalRotations > 0) {
		moveStep();
		waitms(5);
		additionalRotations--;
	}
	if (ACSR & (1 << ACO)) {
		// bandgap (1.1V) is higher than battery voltage after voltage divider
		// -> battery low
		batteryLow = 1;
	}
	stopMotor();
	// disable ADC and voltage divider
	PORTD |= (1 << PD4);
	ACSR |= (1 << ACD);
	ACSR &= ~(1 << ACBG);
}

int main(void) {
	neededRotations15 = 0;
	// disable analog comparator to save energy
	ACSR |= (1 << ACD);
	DDRD = 0b00110000;
	DDRB = 0b00011100;
	// enable all pullups on unused pins
	PORTD = ~0b00110000;
	PORTB = ~0b00011110;
	// disable voltage divider
	PORTD |= (1 << PD4);
	// disable digital input buffer on PB1 (AIN1)
	DIDR |= (1 << AIN1D);
	// configure timer 1 to use as wake-up call each minute
	// prescaler = 64, CTC mode
	OCR1A = 62499; // -> 15 compare matches per minute
	TCCR1B |= (1 << CS11) | (1 << CS10) | (1 << WGM12);
	TIMSK |= (1 << OCIE1A);
	// configure sleep mode
	set_sleep_mode(SLEEP_MODE_IDLE);
	sei();
	while (1) {
		sleep_mode()
		;
		if (minuteFlag)
			moveOneMinute();
		if (batteryLow) {
			// battery voltage is too low
			// -> disable all periphery and enter deep sleep mode
			stopMotor();
			TCCR1B = 0;
			cli();
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_mode()
			;
			// should never reach this point
			for (;;)
				;
		}
	}
}
ISR(TIMER1_COMPA_vect) {
	minuteParts++;
	if (minuteParts >= 15) {
		minuteFlag = 1;
		minuteParts -= 15;
	}
}
