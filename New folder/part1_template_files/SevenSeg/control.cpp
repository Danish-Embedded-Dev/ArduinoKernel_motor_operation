///////////////////////////////////////////////////////////////////////////////
/// CONTROL.H
///
/// Control module. This passes information to the LED driver (and in theory
/// could be to anywhere else) via the message queue
///
///////////////////////////////////////////////////////////////////////////////

#include "control.h"
#include "kernel.h"
#include "common.h" // we need the message ID.

typedef struct _TIMERSTRUCT
{
	Kernel::OSTimer *	LEDTimer;
	Kernel::OSTimer *	SevenSegTimer;
} TIMERSTRUCT;

typedef TIMERSTRUCT * PTIMERSTRUCT;


// Prototype the control task function here as it does not need to be
// seen outside this module

void ControlTask(void * context);

///////////////////////////////////////////////////////////////////////////////
/// CONTROLInitialize
///
/// This is called once at system startup. It initializes the control
/// module (by registering a task to run repetitively) and then exits. This
/// function needs to be seen outside the module, so it is prototyped in the
/// header file.
///
///////////////////////////////////////////////////////////////////////////////

void CONTROLInitialize(void)
{
	// 1) Each test code block needs its own timer. We thus create a structure
	//    containing pointers to two timer objects. Then we create two new
	//    non-blocking timer objects and assign them to the pointers in the structure
	//    This allows us to pass a single pointer to the task handler.
	//
	//    This is not the only way of achieving this. We could use static
	//    or module-global variables. The memory burden is the same.
	//    We need the timer objects to enable the delay between messages sent
	//    to the LED driver We declare this pointer as static
	//    as we need it to stay in place once this function exits.
	//
	//    We only want to do this once.
	//
	//    We should really check the pointers to see if they were allocated
	//    successfully by the OS.

	PTIMERSTRUCT taskcontext=new TIMERSTRUCT;
	taskcontext->LEDTimer=new Kernel::OSTimer(750);	// times out in 750ms
	taskcontext->SevenSegTimer=new Kernel::OSTimer(300); // times out in 300ms

	//
	// 2) Register our repetitive task. We pass the user parameter 'context' as a
	//    pointer to our timer structure. Note that the task handler now takes 'ownership'
	//    of the timer pointer structure. If it required to release the memory associated with it,
	//    this would take place here. However, this task is running until someone turns the power
	//    off, so we don't have to worry here about freeing memory. As the pointer is generic, we use
	//    casting in our ControlTask function to recover the timer object.

	Kernel::OS.TaskManager.RegisterTaskHandler(ControlTask,(void *)taskcontext);

}

//////////////////////////////////////////////////////////////////////////////
/// ControlTask
///
/// This is our main control task. It must not block, as the system as a whole
/// is single-tasked.
///
/// It is passed the timer in the context variable. We could use a module global
/// here, but this is just to demonstrate one use of the context variable
///
//////////////////////////////////////////////////////////////////////////////

void ControlTask(void * context)
{
	// Code in this task function CAN NOT BLOCK. If it blocks, it will grab the
	// CPU and other tasks will not be able to run.

	// Any tasks must take as little time as possible.

	static int ledstate=0;	// declared static as we want to preserve its value across calls

	PTIMERSTRUCT	timers = static_cast<PTIMERSTRUCT>(context);

	if(timers->LEDTimer->isExpired()) {

		// CODE FOR TESTING THE SUBSYSTEMS. YOU MAY WELL NEED TO CHANGE THIS IN YOUR FINAL DESIGN
		//-----------------------------------------------------------------------------------------

		// The code in the section below will be executed once, when the timer has expired.
		// Note that this will not happen EXACTLY when the timer is expired, but the next time
		// this task function runs AFTER the timer has expired. Ergo, it is NOT an 'accurate'
		// timer at all.

		// It simply flips the value of ledstate: if ledstate==0 it changes to 1, if 1 it is changed to 0
		// Because ledstate is declared static, it's value is held over subsequent calls to ControlTask.

		ledstate=(ledstate==1)?0:1;

		// Post the value of ledstate in a message. Any function subscribed to MSG_ID_CHANGE_LED will
		// then be called with the value of 'ledstate' as a parameter. Note that the parameter is
		// declared as a 'void *' - this is technically a pointer type. However, a pointer is a 16 bit
		// integer (in this microprocessor). So we can set it to any value between 0-65535. As long
		// as we don't treat it like a pointer, we can set it to any value!

		Kernel::OS.MessageQueue.Post(MSG_ID_CHANGE_LED, (void *)ledstate, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);

		//----------------------------------------------------------------------------------------------

		// The timer needs to be reset. If it isn't, it will always be expired and the code within
		// the 'if' statement will run every time the ControlTask is called by the task manager

		timers->LEDTimer->Set(750);

	}

	// Now we can check the 7 seg timer

	if(timers->SevenSegTimer->isExpired()) {

		//-----------------------------------------------------------------------------------------

		// The code in the section below sends messages, once each time the timer expires, to the
		// seven segment driver. It sends an integer where the bottom 8 bits are used, all other
		// bits should be set to zero.

		// The bits 4 to 0 represent the number displayed. If a message with the value of '0' is
		// sent, then '0' is displayed. Values 1-8 display numbers 1-9, the value 9 displays 'b'
		// and the value 10 displays 'E'.
		// Bit 4 represents the value of the decimal point. If set, the decimal point is
		// illuminated. If cleared, the decimal point is cleared.
		//
		// Message value	DP state	Display				Message value	DP state	Display
		//		0x00			Off			0					0x10		On			0
		//		0x01			Off			1					0x11		On			1
		//		0x02			Off			2					0x12		On			2
		//		0x03			Off			3					0x13		On			3
		//		0x04			Off			4					0x14		On			4
		//		0x05			Off			5					0x15		On			5
		//		0x06			Off			6					0x16		On			6
		//		0x07			Off			7					0x17		On			7
		//		0x08			Off			8					0x18		On			8
		//		0x09			Off			9					0x19		On			9
		// 		0x0a			Off			b					0x1a		On			b
		//		0x0b			Off			E					0x1b		On			E
		//
		// If the values of the bits 3-0 are outside of the range 0-0x0a, then the display should be blank

		// The code here sequences through the ranges above

		static unsigned int v=0;

		// post the current value as a message

		Kernel::OS.MessageQueue.Post(MSG_ID_CHANGE_7SEG, (void *)v, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);

		// update the value of v for the next cycle. Increments it from zero. We don't skip the
		// invalid values so we can check that the display blanks correctly (except for d.p.)

		if(++v > 0x1b) {
			v=0x00;
		}

		//----------------------------------------------------------------------------------------------

		// The timer needs to be reset. If it isn't, it will always be expired and the code within
		// the 'if' statement will run every time the ControlTask is called by the task manager

		timers->SevenSegTimer->Set(300);
	}
}
