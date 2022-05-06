///////////////////////////////////////////////////////////////////////////////
/// KEYPAD.CPP
///
/// Keyboard module
///
/// Dr J A Gow / Dr M A Oliver 2022
///
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "keypad.h"
#include "kernel.h"
#include "iic.h"

#define KEY_ADDR_IIC	0x40

//
// This enum defines the states used by the state machine

typedef enum _KEYSTATE {

	KEY_IDLE,
	KEY_PRESSDETECTED,
	KEY_PRESSED,
	KEY_RELEASEDETECTED

} KEYSTATE;

// The single task timer used in this module

static Kernel::OSTimer	KeyTimer(10);

//
// Forward definition of keypad task handler

void KEYTaskHandler(void * context);

//
// Exported functions

///////////////////////////////////////////////////////////////////////////////
/// KEYInitializeKeypad
///
/// This is called once at system startup. It initializes the keypad driver.
/// This module is the only place where hardware related to the keypad is
/// directly accessed, so here is where we set the IO parameters. We then
/// register the message handler so it picks up the messages posted from
/// the control application
///
///////////////////////////////////////////////////////////////////////////////

void KEYInitializeKeypad(void)
{
	unsigned char iicreg[2]; // space to put our required I2C data in.

	// Configure the port expander. We want GPIA0,1 and 2 as outputs
	// We also need GPIA3-7 as inputs. We can then usefully construct
	// these into a byte we only need to read once.

	iicreg[0]=0x00;		// IODIRA
	iicreg[1]=0xf8;		// bottom 3 pins output
	IICWrite(KEY_ADDR_IIC,iicreg,2);

	// Now, to start with we want only the LSB low (remember
	// keypad has reverse logic because unfortunately the hardware designer
	// slipped up and pulled EVERYTHING high. If you design hardware, be
	// sympathetic to your firmware designers!

	iicreg[0]=0x12;		// GPIOA
	iicreg[1]=0x06;		// bottommost bit zero
	IICWrite(KEY_ADDR_IIC,iicreg,2);

	// Now, because the hardware designer pulled everything
	// high and used inverse logic, we set the relevant bits in
	// the IPOLA register to put it back to rights

	iicreg[0]=0x02;
	iicreg[1]=0b01111000;
	IICWrite(KEY_ADDR_IIC,iicreg,2);

	// Register the task handler. We do not need to pass any context
	// as in this module, our timer is declared with the scope limited
	// to this module

}

//////////////////////////////////////////////////////////////////////////////
/// KEYTaskHandler
///
/// This is our main task handler for the keypad. It does this in a polled
/// mode. It simply uses a delay and a state machine to debounce, with a map
/// to translate keys to values
///
/// The 'context' parameter is unused in this function
///
//////////////////////////////////////////////////////////////////////////////

void KEYTaskHandler(void * context)
{
	unsigned char matrix;					        // Key state in this current cycle
	unsigned char iicreg[2];				      // IIC data buffer
	static unsigned char lastpressed;		  // needs to be remembered across calls to KEYTaskHandler
	static KEYSTATE keystate = KEY_IDLE;	// needs to hold state across calls to KEYTaskHandler

	// The first thing we need to do is read back the port value
	//
	// We can't use IIC repeat starts here because we don't know if anything
	// else will want to use I2C in the meantime, between calls to this task
  // Following this operation, the value of Port A will be stored in the variable 'matrix'
	iicreg[0]=0x12;						        // GPIOA register address
	IICWrite(KEY_ADDR_IIC,iicreg,1);	// write the address.
	IICRead(KEY_ADDR_IIC,&matrix,1);	// read the value

	// then check the state machine

	switch(keystate) {
		case KEY_IDLE:
    {
      // Examine the value of the variable matrix to see if a 
      // key has been pressed
      // For this exercise, double keys will be ignored.


      // If a press is detected, change state and set the 
      // debounce timer. For future comparisons, save the current
      // state of matrix.


      // Otherwise move onto the next column. Set the current column
      // to logic 1 and the next column to logic 0. Wrap round to
      // the first column if necessary
      unsigned int col;
      // TO DO.......

			// Then write the column to the I2C
			iicreg[1]=col;
			IICWrite(KEY_ADDR_IIC,iicreg,2);
      break;
    }
		
	  case KEY_PRESSDETECTED:

      // check if the debounce timer is expired. If the same key is pressed, accept it. If the key
      // is released, or a different key is pressed, return to idle where it will be picked up

			if(KeyTimer.isExpired()) {

				// Check the key. It is considered to be debounced if it has not changed value.
        // If the key is debounced:-
        // - Post the value of the key to the KEY_PRESSED message.
        // - Post the value of the key to the CHANGE_7SEG message.
        // Move to the KEY_PRESSED state.

        // If the value of the key has changed, move the the KEY_IDLE state

			}   
      break;

		case KEY_PRESSED:	// check for a key release. 
      // Check the value of the key. Use this to check for a key release.

      // If the key is released:-
      // - Post the value of the (previously debounced) key to the KEY_RELEASED message
      // - Post the value of d.p. to the CHANGE_7SEG message.
      // - Transition to KEY_IDLE 

      // If the key remains pressed, remain in the KEY_PRESSED state
			break;


		default:			
      // catch-all. We should never get here
		  // but this gives us belt and braces.
			keystate=KEY_IDLE;
			break;
	}
}
