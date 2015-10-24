/*Copyright (c) 2015, Adam Taylor
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project*/

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xstatus.h"
#include "xintc.h"
#include "xil_exception.h"

#define INTC_DEVICE_ID		  XPAR_INTC_0_DEVICE_ID
#define INTC_DEVICE_INT_ID	  XPAR_INTC_0_TMRCTR_0_VEC_ID

void print(char *str);
int IntcExample(u16 DeviceId);
int SetUpInterruptSystem(XIntc *XIntcInstancePtr);
void DeviceDriverHandler(void *CallbackRef);

static XIntc InterruptController;
volatile static int InterruptProcessed = FALSE;

int main()
{
    init_platform();

    print("Int Sim Example\n\r");
    IntcExample(INTC_DEVICE_ID);

    cleanup_platform();
    return 0;
}

int IntcExample(u16 DeviceId)
{
	int Status;

	Status = XIntc_Initialize(&InterruptController, DeviceId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XIntc_SelfTest(&InterruptController);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = SetUpInterruptSystem(&InterruptController);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XIntc_SimulateIntr(&InterruptController, INTC_DEVICE_INT_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while (1)
	{

		if (InterruptProcessed) {
			break;
		}
	}

	return XST_SUCCESS;
}

int SetUpInterruptSystem(XIntc *XIntcInstancePtr)
{
	int Status;

	Status = XIntc_Connect(XIntcInstancePtr, INTC_DEVICE_INT_ID,
				   (XInterruptHandler)DeviceDriverHandler,
				   (void *)0);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XIntc_Start(XIntcInstancePtr, XIN_SIMULATION_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XIntc_Enable(XIntcInstancePtr, INTC_DEVICE_INT_ID);

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler)XIntc_InterruptHandler,
				XIntcInstancePtr);

	Xil_ExceptionEnable();

	return XST_SUCCESS;

}

void DeviceDriverHandler(void *CallbackRef)
{
	print("Int Simulated in handler\n\r");
	InterruptProcessed = TRUE;

}
