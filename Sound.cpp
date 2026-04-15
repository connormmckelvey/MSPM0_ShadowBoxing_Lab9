// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// your name
// your data 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"

const uint8_t* SoundArr;
uint32_t SoundCount;

void SysTick_IntArm(uint32_t period, uint32_t priority){
  SysTick->CTRL = 0; // turn off systick while initilizing
  SysTick->LOAD = period - 1; // load in systick timer value
  SysTick->VAL = 0; // clears count and allows new load to go in
  SCB->SHP[1] = SCB->SHP[1]&(~0xC0000000)|(priority << 30);
  SysTick->CTRL = 0;
}



// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(void){

  //initilizing systick
  SysTick_IntArm(7272, 0);

  //enabling io
  IOMUX->SECCFG.PINCM[PB0INDEX] = 0x00000081; // GPIO output
  IOMUX->SECCFG.PINCM[PB1INDEX] = 0x00000081; // GPIO output
  IOMUX->SECCFG.PINCM[PB2INDEX] = 0x00000081; // GPIO output
  IOMUX->SECCFG.PINCM[PB3INDEX] = 0x00000081; // GPIO output
  IOMUX->SECCFG.PINCM[PB4INDEX] = 0x00000081; // GPIO output

  //enabling output n shi
  GPIOB->DOE31_0 &= ~0x0000001F;
  GPIOB->DOE31_0 |= 0x0000001F;
}

extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz
  // output one value to DAC if a sound is active
  if(SoundCount != 0){
    DAC5_Out(*SoundArr);
    SoundArr++;
    SoundCount--;
  }
  else{
    SysTick->CTRL = 0; //disabling systick once sound is done
  }

}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Start(const uint8_t *pt, uint32_t count){

  //setting pointers
  SoundArr = pt;
  SoundCount = count;

  //enabling systick
  SysTick->CTRL = 0x07; // setting clk src, interrupt enable, and enabling systick
  SysTick->LOAD = 7272 - 1; // load in systick timer value
  SysTick->VAL = 0; // clears count and allows new load to go in
}

// void Sound_Shoot(void){
// // write this
//   Sound_Start( shoot, 4080);
// }

void Sound_Miss(){

}

void Sound_Hit(){
  
}

void Sound_Bell(){
  
}

void DAC5_Out(uint32_t data){
  GPIOB->DOUTCLR31_0 = 0x0000001F;
  GPIOB->DOUT31_0 |= data;
}