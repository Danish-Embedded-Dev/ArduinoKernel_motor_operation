///////////////////////////////////////////////////////////////////////////////
/// taskring.cpp
///
/// Simple task scheduler for single-tasking applications
///
/// Dr J A Gow 2020
///
///////////////////////////////////////////////////////////////////////////////

#include "taskring.h"
#include <stdlib.h>

//
// task ring internals

namespace Kernel {

	// Task state structure

	typedef class TASKSTATE * 	PTASKSTATE;
	class TASKSTATE {
		public:
			PFNTASKHANDLER		handler;
			void *				context;
			PTASKSTATE			pNext;
			TASKSTATE(PFNTASKHANDLER handler, void * context) : handler(handler),context(context),pNext(NULL) {};
	};

	// Task internal structure

	typedef class TASKINTERNALS *	PTASKINTERNALS;
	class TASKINTERNALS {
		public:
			PTASKSTATE	pHead;
			PTASKSTATE	pCur;
			TASKINTERNALS() : pHead(NULL),pCur(NULL) {};
	};

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

	TaskRing::TaskRing(void)
	{
		this->internals=new TASKINTERNALS;
	}

	///////////////////////////////////////////////////////////////////////////////
	/// Get
	///
	/// Obtain the singleton class instance
	///
	/// @scope: PUBLIC
	/// @context: ANY
	/// @param: none
	/// @return: reference to singleton class
	///
	//////////////////////////////////////////////////////////////////////////////

	static TaskRing& TaskRing::Get(void)
	{
		static TaskRing tr;
		return tr;
	}

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

	void TaskRing::Loop(void)
	{
		PTASKINTERNALS internal=(PTASKINTERNALS)(this->internals);

		if(internal->pCur==NULL) {
			internal->pCur=internal->pHead;
		}

		if(internal->pCur) {
			internal->pCur->handler(internal->pCur->context);
			internal->pCur=internal->pCur->pNext;
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	/// TASKRegisterTaskHandler
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

	int TaskRing::RegisterTaskHandler(PFNTASKHANDLER handler, void * context)
	{
		int rc=-1;
		if(handler) {
			PTASKSTATE pNew = new TASKSTATE(handler,context);
			PTASKINTERNALS internal=(PTASKINTERNALS)(this->internals);
			if(pNew) {
				pNew->pNext=internal->pHead;
				internal->pHead=pNew;
				rc=0;
			}
		}
		return rc;
	}
}
