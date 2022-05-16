//LED DRIVE CIRCUIT


#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

//other definitions

#define OC_RED_LED		(18)  	//B18 TPM2_CH0 is ALT3
#define OC_INFRARED_LED (1)  	//B18 TPM2_CH1 is ALT3
#define MASK(X)  (1<<X)


void init_LED();
void init_Timer1();
void init_Timer2();


int main(void) {

	init_LED();
	init_Timer1();

//delay period between red and IR successive pulses
	for (int delay=0; delay<1045; delay++);//550us
	init_Timer2();


	while(1){}
}

void init_LED(){

	SIM->SCGC5 |=SIM_SCGC5_PORTA_MASK|SIM_SCGC5_PORTB_MASK|SIM_SCGC5_PORTC_MASK|SIM_SCGC5_PORTD_MASK|SIM_SCGC5_PORTE_MASK;
	PORTB->PCR[OC_RED_LED ] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[OC_RED_LED ] |= PORT_PCR_MUX(3);
	PORTC->PCR[OC_INFRARED_LED ] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[OC_INFRARED_LED ] |= PORT_PCR_MUX(4);


}



void init_Timer1(){
         //Clock gate for FTM2C0
	SIM->SCGC6 |=SIM_SCGC6_FTM2_MASK;	//*******FTM2 channel 0
	//Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TIMESRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01
	//Configure registers
	FTM2->MOD= 0x147; //set to 2ms


        // FOR RED_LED
	FTM2->CONTROLS[0].CnSC |= FTM_CnSC_MSB(1) | FTM_CnSC_ELSB(1) ;
	FTM2->CONTROLS[0].CnSC |= FTM_CnSC_CHF_MASK;  //clear spurious interrupts
	FTM2->CONTROLS[0].CnV =0x24;


        FTM2->SC |=  FTM_SC_TOF_MASK | FTM_SC_PS(7) | FTM_SC_TOIE_MASK  ;
        FTM2->SC |= FTM_SC_CLKS(1); //enable internal clock to run

        //NVIC FOR RED_LED
	NVIC_ClearPendingIRQ(FTM2_IRQn);
	NVIC_SetPriority(FTM2_IRQn, 1);
	NVIC_EnableIRQ(FTM2_IRQn);

}

void init_Timer2(){
        //Clock gate for FTM0C0
	SIM->SCGC6 |=SIM_SCGC6_FTM0_MASK;	//*******FTM0 channel 0
	//Select clock source in SIM_SOPT
	SIM->SOPT2 |= SIM_SOPT2_TIMESRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01
	//Configure registers
	FTM0->MOD= 0x147; //set to 2ms

       // FOR IR_LED

	FTM0->CONTROLS[0].CnSC |= FTM_CnSC_MSB(1) | FTM_CnSC_ELSB(1) ;
	FTM0->CONTROLS[0].CnSC |= FTM_CnSC_CHF_MASK;  //clear spurious interrupts
	FTM0->CONTROLS[0].CnV =0x24;  //for 2us;

        FTM0->SC |=  FTM_SC_TOF_MASK | FTM_SC_PS(7) | FTM_SC_TOIE_MASK  ;
        FTM0->SC |= FTM_SC_CLKS(1); //enable internal clock to run


       //NVIC FOR IR_LED
	NVIC_ClearPendingIRQ(FTM0_IRQn);
	NVIC_SetPriority(FTM0_IRQn, 2);
	NVIC_EnableIRQ(FTM0_IRQn);

}


void FTM2_IRQHandler(){
	//optionally check the flag
	if (FTM2->STATUS & FTM_STATUS_CH0F_MASK){		//alternative check using the status flag
	//if (TPM2->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){
		FTM2->CONTROLS[0].CnSC |=FTM_CnSC_CHF_MASK;//clear flag
	}
	else{
	 //PTB->PSOR |= MASK(RED_LED ) ;
	 FTM2->SC |= FTM_SC_TOF_MASK ; //clear the interrupt
	}
}



void FTM0_IRQHandler(){
	//optionally check the flag
	if (FTM0->STATUS & FTM_STATUS_CH0F_MASK){		//alternative check using the status flag
	//if (TPM2->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK){
		FTM0->CONTROLS[0].CnSC |=FTM_CnSC_CHF_MASK;//clear flag
	}
	else{
	 //PTB->PSOR |= MASK(RED_LED ) ;
	 FTM0->SC |= FTM_SC_TOF_MASK ; //clear the interrupt
	}
}

