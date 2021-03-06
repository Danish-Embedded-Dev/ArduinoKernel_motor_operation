///////////////////////////////////////////////////////////////////////////////
/// INTERRUPTS.H
///
/// Kernel interrupt management
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

///////////////////////////////////////////////////////////////////////////////
/// INTDisableMasterInterrupts
///
/// Disable global interrupts
///
/// @context: TASK
/// @scope: EXPORTED
/// @param: none
/// @return: none
///
//////////////////////////////////////////////////////////////////////////////

void INTDisableMasterInterrupts(void);

///////////////////////////////////////////////////////////////////////////////
/// INTEnableMasterInterrupts
///
/// Enable global interrupts
///
/// @context: TASK
/// @scope: EXPORTED
/// @param: none
/// @return: none
///
//////////////////////////////////////////////////////////////////////////////

void INTEnableMasterInterrupts(void);



#endif
