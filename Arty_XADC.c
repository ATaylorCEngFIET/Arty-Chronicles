/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xsysmon.h"
#define RX_BUFFER_SIZE 6
#define xadc XPAR_SYSMON_0_DEVICE_ID
XSysMon xadc_inst;


char *channel[] = {"Temp","VCCInt","VCCAux","VRefP","VRefN","VBram"};
int sample[6] = {0,1,2,4,5,6};

int main()
{
	int Index;
	XSysMon *xadc_inst_ptr = &xadc_inst;
	u32 XADC_Buf[RX_BUFFER_SIZE];
	XSysMon_Config *xadc_config;

	init_platform();

    print("XADC Example Up\n\r");

    xadc_config =  XSysMon_LookupConfig(xadc);
    XSysMon_CfgInitialize(xadc_inst_ptr, xadc_config,xadc_config->BaseAddress);

    XSysMon_SetSequencerMode(xadc_inst_ptr,XSM_SEQ_MODE_SAFE);
    XSysMon_SetAlarmEnables(xadc_inst_ptr, 0x00000000);


    XSysMon_SetTempWaitCycles(xadc_inst_ptr, 0x00000340);
    XSysMon_EnableTempUpdate(xadc_inst_ptr);


    XSysMon_SetSeqChEnables(xadc_inst_ptr, XSM_CH_TEMP|XSM_CH_VCCINT|XSM_CH_VCCAUX|XSM_CH_VREFP|XSM_CH_VREFN|XSM_CH_VBRAM);
    XSysMon_SetSequencerMode(xadc_inst_ptr,XSM_SEQ_MODE_SAFE);

    while(1){

    	XSysMon_GetStatus(xadc_inst_ptr); /* Clear the old status */

    	for(Index =0; Index <RX_BUFFER_SIZE; Index++){
    		while ((XSysMon_GetStatus(xadc_inst_ptr) & XSM_SR_EOS_MASK) !=XSM_SR_EOS_MASK);
    		XADC_Buf[Index] = XSysMon_GetAdcData(xadc_inst_ptr, sample[Index]);
    	}

    	for(Index =0; Index <RX_BUFFER_SIZE; Index++){

    		printf("Raw %s %d\n",channel[Index], (int)(XADC_Buf[Index]>>4)); //shift as 12 bits are the MS 12 bits
    		if (Index == 0) {
    			printf("Temperature %s %f\n",channel[Index], XSysMon_RawToTemperature(XADC_Buf[Index]));
    		} else {
    			printf("Voltage %s %f\n",channel[Index], XSysMon_RawToVoltage(XADC_Buf[Index]));
    		}
    	}

    }
    cleanup_platform();
    return 0;
}
