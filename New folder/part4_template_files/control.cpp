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
	Kernel::OSTimer *	TestRPSTimer;
} TIMERSTRUCT;

typedef TIMERSTRUCT * PTIMERSTRUCT;

// the rps value needs to be seen by more than one function, so make it
// module scope.

static int demandrps=RPS_MIN;

// Prototype the control task function and encoder callback here as it does not need to be
// seen outside this module

void CTRLEncoderClicked(void * context);	// someone's tweaked the encoder
void CTRLNewRPS(void * context);			// if someone enters rps from keypad
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
	taskcontext->TestRPSTimer=new Kernel::OSTimer(1000); // times out in 200ms

	// Register to receive messages from the encoder.

	Kernel::OS.MessageQueue.Subscribe(MSG_ID_ENCODER, CTRLEncoderClicked);

	// Register to receive RPS updates from keypad

	Kernel::OS.MessageQueue.Subscribe(MSG_ID_NEW_RPS_KEYPAD, CTRLNewRPS);

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

	// Note: We no longer use the control function test code for the 7-seg. Why? We have already tested it,
	// and now the keyboard driver can directly post the messages to the 7-seg to display according to
	// specification. We leave the timer object available, but change its name. It is not doing anything
	// at the moment, but we keep it as we may need it later.

	// Check the RPS test timer. This just increments the RPS display by one.

	if(timers->TestRPSTimer->isExpired()) {

		static int actualrps=0;		// declared static as we want it to survive repeated calls to this task

		actualrps=(++actualrps)>RPS_MAX?0:actualrps; // increment by 1 unless > RPS_MAX.

		Kernel::OS.MessageQueue.Post(MSG_ID_NEW_ACTUAL_RPS, (void *)actualrps, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);

		timers->TestRPSTimer->Set(1000);
	}

}

////////////////////////////////////////////////////////////////////////////////
/// CTRLEncoderClicked
///
/// Callback from the message queue if someone whizzed the encoder. The context
/// will contain either 1 (if the encoder is clockwise) or -1 if the encoder
/// is rotated anticlockwise. We update the RPS displayed, if we are able to
///
/// @context: TASK
/// @scope: INTERNAL
/// @param: void * context - 0 or 1 depending if anticlockwise or clockwise
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void CTRLEncoderClicked(void * context)
{
  int delta = (int)context;

  // TODO: Increment demandrps by delta ensuring it lies within the 
  // bounds of RPS_MIN to RPS_MAX
  
  // TODO: POST the value of demandrps to the NEW DEMAND RPS Message ID
 
}

////////////////////////////////////////////////////////////////////////////////
/// CTRLNewRPS
///
/// Callback from the message queue if someone entered a new RPS from
/// the keypad. This comes from the display module and will already have
/// been validated
///
/// @context: TASK
/// @scope: INTERNAL
/// @param: void * context - 0 or 1 depending if anticlockwise or clockwise
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void CTRLNewRPS(void * context)
{
	demandrps=(unsigned int)context;

	// we post this back to the display, to be displayed.
	// Seems convoluted, but this gives us power of veto if for some
	// reason we can not accept the keypad value.

	Kernel::OS.MessageQueue.Post(MSG_ID_NEW_DEMAND_RPS, (void *)demandrps, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);

}
