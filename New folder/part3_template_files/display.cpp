////////////////////////////////////////////////////////////////////////////////
/// DISPLAY.H
///
/// Display module
///
/// Dr J A Gow / Dr M A Oliver 2022
///
////////////////////////////////////////////////////////////////////////////////

#include "display.h"
#include "common.h"
#include <kernel.h>
#include <LiquidCrystal_I2C.h>


//
// This enum defines the states used by the state machine

typedef enum _DISPSTATE {

	DISPSTATE_REFSH,
	DISPSTATE_IDLE,
	DISPSTATE_UPDATING,
	DISPSTATE_VALIDATE,
	DISPSTATE_ERROR

} DISPSTATE;


// One module-wide instance of the display object, as declared in the
// library.

LiquidCrystal_I2C lcd(DISP_I2C_ADDR,16,2);

// Two module variables containing the demanded and
// actual RPM to display
static unsigned int ActualRPM = 0;
static unsigned int DemandRPM = 0;

// Another module variable contains the unvalidated
// entered RPM value.
static unsigned int EnteredRPM = 0;	// value entered

// The character sequence entered by the user. We keep this because
// it is needed in both task and message handler
static char numarr[5];

// Display state variable
DISPSTATE state = DISPSTATE_REFSH;

// Prototype of display task functions

void DISPTask(void * context);			// display task handler
void DISPUpdateRPM(void * context);		// message handler for actual RPM updates
void DISPUpdateDemandRPM(void * context);	// message handler for demand RPM updates
void DISPKeyPressed(void * context);		// keypad update pressed

////////////////////////////////////////////////////////////////////////////////
/// DISPInitialize
///
/// Initialize the display
///
/// @scope: EXPORTED
/// @context: TASK
/// @param: none
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void DISPInitialize(void)
{
  // Preliminary setup
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Starting.."));

  // Capture messages updating the actual RPM and the demand RPM
  // Also capture key-press messages.
  // TODO: Add your three subscriptions.

  // Register the task for the display
  // TODO: Add this task registration

}

////////////////////////////////////////////////////////////////////////////////
/// DISPTask
///
/// Main task for the display module
///
/// @scope: INTERNAL
/// @context: TASK
/// @param: none (context is null)
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void DISPTask(void * context)
{
	static Kernel::OSTimer *errtimer;	// timeout for error.

	switch(state) {

		case DISPSTATE_REFSH:		// Refresh the display
      // TODO: Write the code to display the Demand and Actual RPM

			break;

		case DISPSTATE_IDLE:
		case DISPSTATE_UPDATING:	
		  // do nothing. We only update when messages arrive asking us to.
			break;

		case DISPSTATE_VALIDATE:
      // TODO: Alter the condition in the following if statement to check
      // whether the EnteredRPM value is out of range or not.	
		  if(1) 
		  {
			  errtimer=new Kernel::OSTimer(2000);
				errtimer->Set(2000);

        // TODO: Add code to display error message
										
				state=DISPSTATE_ERROR;
      } else {
        // tell anyone who needs to know that the RPM has been updated via the keypad.

        // TODO: In the message queue, publish the EnteredRPM value 
        // to your "New RPM from Keypad" message ID

        state=DISPSTATE_REFSH;
		  }
			break;

		case DISPSTATE_ERROR:		
		  if(errtimer->isExpired()) {
		    // we won't need the timer now, free mem associated with it.
				delete errtimer;

        // refresh the display to EnteredRPM
        // TODO: Display the EnteredRPM on the display
        // and place a blinking cursor over the first character.

        // Return to updating state in hope that a valid Demand RPM may be entered
				state=DISPSTATE_UPDATING;
      }
		  break;

		// a catch-all, we should never get here.
		default: 					
		  state=DISPSTATE_IDLE;
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////
/// DISPUpdateRPM
///
/// Responds to messages coming in with an updated actual RPM. We
/// check if there is any change, and if so, display it. We can only
/// do this if in DISPSTATE_IDLE
///
/// @context: TASK
/// @scope: INTERNAL
/// @param: void * context - RPM value cast to unsigned int
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void DISPUpdateRPM(void * context)
{
	unsigned int newrpm=(unsigned int)context;

	// NOTE: The display is slow. We may thus want to
	// control the update rate, and not update every time
	// a change is made.

	if(state==DISPSTATE_IDLE || state==DISPSTATE_REFSH) {
		// we only update the display if we need to.
		if(newrpm!=ActualRPM) {
			ActualRPM=newrpm;
			char tempstr[6];
			sprintf(tempstr,"%3.3d",ActualRPM);
			lcd.setCursor(12,1);
			lcd.print(tempstr);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
/// DISPUpdateDemandRPM
///
/// Responds to messages coming in to change the displayed demand RPM. We
/// check if there is any change, and if so, display it. We can only
/// do this if in DISPSTATE_IDLE
///
/// @context: TASK
/// @scope: INTERNAL
/// @param: void * context - RPM value cast to unsigned int
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void DISPUpdateDemandRPM(void * context)
{
	unsigned int newrpm=(unsigned int)context;
  
	// NOTE: The display is slow. We may thus want to
	// control the update rate, and not update every time
	// a change is made. So we only update if what is sent
	// in is different

	// TODO: Implemente the remainder of the function to
  // update the demanded RPM 
}


////////////////////////////////////////////////////////////////////////////////
/// DISPKeyPressed
///
/// Someone has pressed a button. We need to read the user's value, and
/// deal with it according to state.
///
/// @context:  TASK
/// @scope: INTERNAL
/// @param: void * context: encoded value of key pressed as unsigned char
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void DISPKeyPressed(void * context)
{
	unsigned char keyval=(unsigned char)context;
	static unsigned int curpos=9;

	switch(state) {

		case DISPSTATE_IDLE:
		case DISPSTATE_REFSH:		// if the key is anything but a numeral, ignore it
		  if(keyval<0x0a) {
				// This is the first press. Set up the display:
				curpos=9;
				sprintf(numarr,"%3.3d",DemandRPM);
				numarr[0]=0x30+keyval;
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print(F("New RPM:"));
				lcd.setCursor(curpos,0);
				lcd.print(numarr);
				lcd.setCursor(++curpos,0);
				lcd.blink();
				state=DISPSTATE_UPDATING;
			}
			break;

		case DISPSTATE_UPDATING:	
      // TODO: Add the code to deal with Enter (#),
      // Backspace (*) and subsequent 0-9 keypresses.
      
		  break;

		// in all other states, we take no action if a key is pressed.

		default:					break;
	}
}
