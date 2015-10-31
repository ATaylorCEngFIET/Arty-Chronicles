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
#include "xtmrctr.h"

#define INTC_DEVICE_ID		 	XPAR_INTC_0_DEVICE_ID
#define INTC_DEVICE_INT_ID	 	XPAR_INTC_0_TMRCTR_0_VEC_ID
#define TMRCTR_DEVICE_ID		XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_CNTR_0	 		0
#define RESET_VALUE	 			0xF0000000


void print(char *str);
int IntcExample();
int SetUpInterruptSystem(XIntc *XIntcInstancePtr, XTmrCtr *XTmrInstancePtr);
void DeviceDriverHandler(void *CallbackRef);

static XIntc InterruptController;
static XTmrCtr TimerCounterInst;
//volatile static int InterruptProcessed = FALSE;

int main()
{
    init_platform();

    print("Timer Sim Example\n\r");
    IntcExample();

    cleanup_platform();
    return 0;
}

int IntcExample()
{
	int Status;

	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XIntc_SelfTest(&InterruptController);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XTmrCtr_Initialize(&TimerCounterInst, TMRCTR_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XTmrCtr_SelfTest(&TimerCounterInst,TIMER_CNTR_0);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

	Status = SetUpInterruptSystem(&InterruptController,&TimerCounterInst);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XTmrCtr_SetHandler(&TimerCounterInst,DeviceDriverHandler,&TimerCounterInst);
	XTmrCtr_SetOptions(&TimerCounterInst, TIMER_CNTR_0, XTC_INT_MODE_OPTION);
	XTmrCtr_SetResetValue(&TimerCounterInst, TIMER_CNTR_0, RESET_VALUE);
	XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_0);

	while (1)
	{


	}

	return XST_SUCCESS;
}

int SetUpInterruptSystem(XIntc *XIntcInstancePtr, XTmrCtr *XTmrInstancePtr)
{
	int Status;

	Status = XIntc_Connect(XIntcInstancePtr, INTC_DEVICE_INT_ID,
				   (XInterruptHandler)DeviceDriverHandler,
				   (void *)XTmrInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XIntc_Start(XIntcInstancePtr, XIN_REAL_MODE);
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
	print("Timer Event\n\r");
	XTmrCtr_SetResetValue(&TimerCounterInst, TIMER_CNTR_0, RESET_VALUE);
	XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_0);

}
