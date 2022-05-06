///////////////////////////////////////////////////////////////////////////////
/// LedDemo
///
/// This is the main module showing an encapsulated LED driver.
///
/// The module containing the LED driver itself is in led.h/led.cpp
/// The module containing the controller that passes messages to the LED to
/// change its state is in control.h/control.cpp
///
///////////////////////////////////////////////////////////////////////////////

#include "kernel.h"
#include "leddriver.h"
#include "ssegdriver.h"
#include "control.h"


//////////////////////////////////////////////////////////////////////////////
/// UserInit
///
/// This function is called once by the kernel on startup
///
/// We use it to initialize our modules
///
/////////////////////////////////////////////////////////////////////////////

void UserInit()
{
	// Order may be important - always check your code.

	LEDInitializeDriver();
	SSEGInitializeDriver();
	CONTROLInitialize();
}
