////////////////////////////////////////////////////////////////////////////////
/// KERNELCLASS.CPP
///
/// Kernel class
///
/// Dr J A Gow 2022
///
////////////////////////////////////////////////////////////////////////////////

#include "KernelClass.h"

namespace Kernel {

///////////////////////////////////////////////////////////////////////////////
/// KernelClass
///
/// CONSTRUCTOR
///
/// Initializes kernel object (statically)
///
///////////////////////////////////////////////////////////////////////////////

KernelClass::KernelClass()
{
}

///////////////////////////////////////////////////////////////////////////////
/// ~KernelClass
///
/// DESTRUCTOR
///
/// In an embedded environment, this is usually never called. However we include
/// for completeness.
///
///////////////////////////////////////////////////////////////////////////////

KernelClass::~KernelClass()
{
	// normally never called in an embedded environment
}

}