#include "fsl_device_registers.h"
static int i = 0;
uint32_t light = 0;
uint32_t mode = 0;
void speed(int mode){
	if ((mode & 0x8) != 0 && (mode & 0x04) != 0){ // 1,1
		GPIOD_PDOR = 0x36;
		for(i = 0; i < 10000; i++);
		GPIOD_PDOR = 0x35;
		for(i = 0; i < 10000; i++);
		GPIOD_PDOR = 0x39;
		for(i = 0; i < 10000; i++);
		GPIOD_PDOR = 0x3A;
		for(i = 0; i < 10000; i++);
	}
	else if ((mode & 0x8) != 0 && (mode & 0x04) == 0){ // 1, 0
		GPIOD_PDOR = 0x36;
		for(i = 0; i < 20000; i++);
		GPIOD_PDOR = 0x35;
		for(i = 0; i < 20000; i++);
		GPIOD_PDOR = 0x39;
		for(i = 0; i < 20000; i++);
		GPIOD_PDOR = 0x3A;
		for(i = 0; i < 20000; i++);
	}
	else if ((mode & 0x8) == 0 && (mode & 0x04) != 0){ // 0, 1
		GPIOD_PDOR = 0x36;
		for(i = 0; i < 30000; i++);
		GPIOD_PDOR = 0x35;
		for(i = 0; i < 30000; i++);
		GPIOD_PDOR = 0x39;
		for(i = 0; i < 30000; i++);
		GPIOD_PDOR = 0x3A;
		for(i = 0; i < 30000; i++);
	}
	else if ((mode & 0x8) == 0 && (mode & 0x04) == 0){ // 0, 0
		GPIOD_PDOR = 0x36;
		for(i = 0; i < 40000; i++);
		GPIOD_PDOR = 0x35;
		for(i = 0; i < 40000; i++);
		GPIOD_PDOR = 0x39;
		for(i = 0; i < 40000; i++);
		GPIOD_PDOR = 0x3A;
		for(i = 0; i < 40000; i++);
	}
}

unsigned short adc_read16b(){
	//ADC0_SC3 = 0x07;
	ADC0_SC1A = 0x00;
	while(ADC0_SC2 & ADC_SC2_ADACT_MASK);
	while(!(ADC0_SC1A & ADC_SC1_COCO_MASK));
	return ADC0_RA; //31539, 32534
}

void PORTA_IRQHandler(void)
{
	mode = GPIOB_PDIR; // read port b(switch)
	light = (adc_read16b()* 33) / 0xFFFF; //read ADC value and convert to decimal
	if(light > 16){
		speed(mode);
	}
	else{
		//do nothing
	}
	//Clear ISFR
	PORTA_ISFR = (1 << 1);

}


int main(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK; /*Enable Port A Clock Gate Control*/
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; /*Enable Port B Clock Gate Control*/
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/
    SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;  /*Enable ADC*/

    //port b
    PORTB_GPCLR = 0x000C0100; /*Configure Port B Pin 2-3 for GPIO*/
    GPIOB_PDDR = 0x00000000; /*Configure Port B Pin 2-3 for Input*/


    //Port D
    PORTD_GPCLR = 0x00FF0100; /*Configure Port D Pins 0-7 for GPIO*/
	GPIOD_PDDR = 0x000000FF; /*Configure Port D Pins 0-7 for Output*/
	GPIOD_PDOR = 0x01; /*Initialize Port D such that only 1 bit is ON*/

    PORTA_PCR4 = 0x0100;  //port A

    //Configure PA1 to trigger interrupts on falling edge input.
    //PORTC_PCR1 = 0xA0100;
    //Configure ADC for 16 bits, and to use bus clock.
    ADC0_CFG1 = 0x0C;
    //Disable the ADC module;
    ADC0_SC1A = 0x1F;
    //Set PB[3:2] and PA[1] for input;
    GPIOA_PDDR |= (0 << 1);  // PC[1] input
    GPIOB_PDDR |= ((0 << 3) | (0 << 2) | (1 <<2) | (1 << 10)); // PB[3:2] input


    PORTA_ISFR = (1 << 1);
    //NVIC_EnableIRQ(PORTA_IRQn);

    for(;;){
    	GPIOB_PTOR |= (1 << 10);
    	PORTA_IRQHandler();
    }
    return 0;
}
