///////////////////////////////////////////////////////////////////////////////
/// TASKTIMERS.H
///
/// Convenient task-time (non-interrupt) timer blocks
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#ifndef TASKTIMERS_H_
#define TASKTIMERS_H_

namespace Kernel {

	//
	// These functions are described as a series of macros that allow easy instantiation
	// of timers

	///////////////////////////////////////////////////////////////////////////////
	/// CREATE_TIMER
	///
	/// Instantiate a timer. This can be scoped global or within a function, as the
	/// timer core is declared static. The macro should be provided with a unique
	/// integer timer ID, and a timeout for the timer in milliseconds.
	///
	/// @context: ANY
	/// @scope: ANUY
	/// @param: t - unique ID for timer
	/// @param: initial timeout value for timeout.
	///
	///////////////////////////////////////////////////////////////////////////////

	#define CREATE_TIMER(t,init) \
		static unsigned long tmr##t=0,time##t=init,freeze##t=0; \
		static unsigned char frozen##t=0;

	///////////////////////////////////////////////////////////////////////////////
	/// SETTIMER
	///
	/// Resets the timeout value of the timer with the supplied ID
	///
	/// @context: ANY
	/// @scope: ANY
	/// @param: t - unique ID for timer
	/// @param: tm - timeout value in ms
	///
	///////////////////////////////////////////////////////////////////////////////

	#define SETTIMER(t,tm) \
	{                      \
		tmr##t=millis();   \
		time##t=tm;        \
	}

	///////////////////////////////////////////////////////////////////////////////
	/// ISEXPIRED
	///
	/// Returns true if the specified timer ID has an expired timeout
	///
	/// @context: ANY
	/// @scope: ANY
	/// @param: t- unique ID of timer
	///
	///////////////////////////////////////////////////////////////////////////////

	#define ISEXPIRED(t) \
		((millis()-tmr##t)> time##t)

	///////////////////////////////////////////////////////////////////////////////
	/// FREEZETIMER
	///
	/// 'Freeze' a timer - i.e. stop it counting down at a specific point
	///
	/// @context: ANY
	/// @scope: ANY
	/// @param: t - unique ID of timer
	///
	//////////////////////////////////////////////////////////////////////////////

	#define FREEZETIMER(t) \
	{   	                   \
		freeze##t=millis();\
		frozen##t=TRUE;    \
	}

	//////////////////////////////////////////////////////////////////////////////
	/// THAWTIMER
	///
	/// 'Thaw' a previously frozen timer - i.e. let it resume
	///
	/// @context: ANY
	/// @scope: ANY
	/// @param: t - unique ID of timer to thaw
	///
	/////////////////////////////////////////////////////////////////////////////

	#define THAWTIMER(t)  \
	{                     \
		tmr##t+=(frozen##t)?(millis()-freeze##t):0; \
	}

}

#endif

