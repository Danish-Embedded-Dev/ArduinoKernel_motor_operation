///////////////////////////////////////////////////////////////////////////////
/// taskring.h
///
/// Simple task scheduler for single-tasking applications
///
/// Dr J A Gow 2020
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _TASKRING_H_
#define _TASKRING_H_

#include "sysincs.h"

typedef void (*PFNTASKHANDLER)(void * context);

// the Arduino 'loop' function is declared with 'C' linkage, not C++

namespace Kernel {

	class TaskRing {

		private:

			friend void ::loop();		// the kernel needs to access the Loop function

			// internals

			void *	internals;

			///////////////////////////////////////////////////////////////////////////////
			/// TASKRing
			///
			/// CONSTRUCTOR, PRIVATE
			///
			/// Initialize the task handler subsystem. This class should be a singleton
			/// within the kernel.
			///
			/// @scope: 	EXPORTED
			/// @context: 	TASK
			/// @param:  	none
			/// @return:	zero for success.
			///
			///////////////////////////////////////////////////////////////////////////////

			TaskRing(void);

			///////////////////////////////////////////////////////////////////////////////
			/// Loop
			///
			/// Called by the kernel at task time to sequentially call the handlers.
			///
			/// @scope:	  EXPORTED
			/// @context: TASK
			/// @param:   none
			///
			///////////////////////////////////////////////////////////////////////////////

			void Loop(void);

		public:

			///////////////////////////////////////////////////////////////////////////////
			/// Get
			///
			/// Return the singleton class
			///
			/// @context: ANY
			/// @scope: PUBLIC
			/// @param: none
			/// @return: reference to single instance of static class
			///
			///////////////////////////////////////////////////////////////////////////////

			static TaskRing& Get(void);


			///////////////////////////////////////////////////////////////////////////////
			/// RegisterTaskHandler
			///
			/// Passed a pointer to a function, and a pointer to some context data,
			/// will register this function to be called by the scheduler at task time. At
			/// all times the context data is 'owned' by the caller
			/// Can block, but will block other tasks. Should not be called in interrupt
			/// context.
			///
			/// @scope:   EXPORTED
			/// @context: TASK
			/// @param:   PFNHANDLER pfnHandler
			/// @param:   (void *) context
			///
			///////////////////////////////////////////////////////////////////////////////

			int RegisterTaskHandler(PFNTASKHANDLER handler, void * context);

	};
}

#endif
