
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"


#define PTB2 (2) //ADC0_SE12
#define PTB10 (10)//ADC1_SE14



void measure_DC_ADC1(void);
void measure_AC_ADC2(void);


void init_ADC(void) ;

char dc_buff[2];
char ac_buff[3];

volatile int result_dc=0;  //the value readin ADC
volatile int result_ac=0;  //the value readin ADC


int main (void) {
	init_ADC();

	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(12);  //trigger first conversion.
	ADC1->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(14);  //trigger first conversion.

	while (1) {

		measure_DC_ADC1();
		measure_AC_ADC2();
	}
}



void init_ADC(void) {

	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	SIM->SCGC3 |= SIM_SCGC3_ADC1_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

	// Select analog for pin
	//For DC
	PORTB->PCR[PTB2] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB2] |= PORT_PCR_MUX(0);
	//for AC
	PORTB->PCR[PTB10] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB10] |= PORT_PCR_MUX(0);


	//16 bit single-ended conversion, when DIFF=0
	ADC0->CFG1 |= ADC_CFG1_MODE(3);		//0 is 8-bit conversion, 3=16bit,
	ADC1->CFG1 |= ADC_CFG1_MODE(3);		//0 is 8-bit conversion, 3=16bit,


	// enable interrupts
	ADC0->SC1[0] |= ADC_SC1_AIEN_MASK ;
	ADC1->SC1[0] |= ADC_SC1_AIEN_MASK ;


	//NVIC DC
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	NVIC_SetPriority(ADC0_IRQn, 3);
	NVIC_EnableIRQ(ADC0_IRQn);
	//NVIC AC
	NVIC_ClearPendingIRQ(ADC1_IRQn);
	NVIC_SetPriority(ADC1_IRQn, 3);
	NVIC_EnableIRQ(ADC1_IRQn);
}



void ADC0_IRQHandler(void){
	if (ADC0->SC1[0] & ADC_SC1_COCO_MASK){
		result_dc = ADC0->R[0];
		//COCO flag is cleared when data is read.
		ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(12);
	}
}

void ADC1_IRQHandler(void){
	if (ADC1->SC1[0] & ADC_SC1_COCO_MASK){
		result_ac = ADC1->R[1];
		//COCO flag is cleared when data is read.
		ADC1->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(14);
	}
}

void measure_DC_ADC1(void){

	printf("%X\n",result_dc);


}

void measure_AC_ADC2(void){

	printf("%X\n",result_ac);


}
