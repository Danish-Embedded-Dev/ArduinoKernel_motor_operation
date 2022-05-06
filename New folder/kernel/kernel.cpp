#include "kernel.h"
#include "mq.h"

namespace Kernel {
	Kernel::KernelClass OS;
}

void setup()
{
	UserInit();
}


void loop(void)
{
	Kernel::OS.MessageQueue.Loop(2);
	Kernel::OS.TaskManager.Loop();
}
