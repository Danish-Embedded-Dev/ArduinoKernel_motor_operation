///////////////////////////////////////////////////////////////////////////////
/// SSEGDRIVER.CPP
///
/// This is the driver for the seven segment display. It is a shell - you
/// should populate with your own code
///
///////////////////////////////////////////////////////////////////////////////

#include "ssegdriver.h"
#include "common.h"
#include "kernel.h"

// prototype the message handler function here.

void SSEGControlMessageHandler(void * context);

// array mapping values to segments - you may need something like that here


///////////////////////////////////////////////////////////////////////////////
/// SSEGInitializeDriver
///
/// This is called once at system startup. It initializes the seven segment
/// display driver.
/// This module is the only place where hardware related to the seven segment
/// display is directly accessed, so here is where we set the IO parameters.
///
///////////////////////////////////////////////////////////////////////////////

void SSEGInitializeDriver(void)
{
	// This is the code that configures the pins on the ATMega328 as
	// outputs WITHOUT CHANGING OTHER PINS (note the use of bitwise-OR)

	// DATA (SER on HC595) is on PORTD bit 4
	// CLK  (SRCLK on HC595) is on PORTB bit 0
	// EN   (ECLK on HC595) is on PORTD bit 7

	DDRD |= 0b10010000;	// set to o/p
	DDRB |= 0b00000001; // set to o/p

	// set EN high, data low, clock low (as initial)

	PORTD |= 0b10000000;
	PORTB &= ~0b00000001;
	PORTD &= ~0b00010000;

	// This line registers the message handler with the OS in order
	// that it can receive messages posted to it under the MSG_ID_CHANGE_7SEG
	// ID

	Kernel::OS.MessageQueue.Subscribe(MSG_ID_CHANGE_7SEG, SSEGControlMessageHandler);
}


///////////////////////////////////////////////////////////////////////////////
/// LEDControlMessageHandler
///
/// This function is called in response to a posted message
/// The 'context' parameter is not used as a pointer, but is cast to
/// an integer, which is used as a boolean. Zero means LED off, nonzero
/// means LED on.
///
///////////////////////////////////////////////////////////////////////////////

void SSEGControlMessageHandler(void * context)
{
	unsigned int value = (unsigned int)context;

	/// your code goes here, as well as above if you ned to declare statics
	/// or module globals.

	// Remember that the logic sense of the outputs of the port expander
	// are INVERTED: a logic '0' will light the segment



}

