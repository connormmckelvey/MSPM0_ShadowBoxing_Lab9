#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "Sound.h"
#include "accel_read.h"

extern "C" {
  void __disable_irq(void);
}

#define LEFT   0
#define RIGHT  1
#define THRESH 700

int32_t REST_P1, REST_P2;
int32_t r1, r2;
int32_t last_gesture;
int32_t match;
int8_t d1 = -1, d2 = -1;


//adc 0 is on PB20
void ADC0_Init(void){
  ADC0->ULLMEM.GPRCM.RSTCTL = 0xB1000003;
  ADC0->ULLMEM.GPRCM.PWREN  = 0x26000001;
  Clock_Delay(24);
  ADC0->ULLMEM.GPRCM.CLKCFG = 0xA9000000;
  ADC0->ULLMEM.CLKFREQ       = 7;
  ADC0->ULLMEM.CTL0          = 0x03010000;
  ADC0->ULLMEM.CTL1          = 0x00000000;
  ADC0->ULLMEM.CTL2          = 0x00000000;
  ADC0->ULLMEM.MEMCTL[0]     = 6;
  ADC0->ULLMEM.SCOMP0        = 0;
  ADC0->ULLMEM.CPU_INT.IMASK = 0;
}


///ADC1 is on PB18
void ADC1_Init(void){
  ADC1->ULLMEM.GPRCM.RSTCTL = 0xB1000003;
  ADC1->ULLMEM.GPRCM.PWREN  = 0x26000001;
  Clock_Delay(24);
  ADC1->ULLMEM.GPRCM.CLKCFG = 0xA9000000;
  ADC1->ULLMEM.CLKFREQ       = 7;
  ADC1->ULLMEM.CTL0          = 0x03010000;
  ADC1->ULLMEM.CTL1          = 0x00000000;
  ADC1->ULLMEM.CTL2          = 0x00000000;
  ADC1->ULLMEM.MEMCTL[0]     = 5;
  ADC1->ULLMEM.SCOMP0        = 0;
  ADC1->ULLMEM.CPU_INT.IMASK = 0;
}

void ADC_ReadBoth(int32_t *p1, int32_t *p2){
  ADC0->ULLMEM.CTL0 |= 0x1;
  ADC0->ULLMEM.CTL1 = (ADC0->ULLMEM.CTL1 & ~0x00000100) | 0x00000100;
  ADC1->ULLMEM.CTL0 |= 0x1;
  ADC1->ULLMEM.CTL1 = (ADC1->ULLMEM.CTL1 & ~0x00000100) | 0x00000100;
  while((ADC0->ULLMEM.STATUS & 0x01) == 0x01){}
  *p1 = ADC0->ULLMEM.MEMRES[0];
  while((ADC1->ULLMEM.STATUS & 0x01) == 0x01){}
  *p2 = ADC1->ULLMEM.MEMRES[0];
}

void Calibrate(void){
  int32_t sum1 = 0, sum2 = 0;
  for(uint32_t i = 0; i < 1000; i++){
    ADC_ReadBoth(&r1, &r2);
    sum1 += r1;
    sum2 += r2;
  }
  REST_P1 = sum1 / 1000;
  REST_P2 = sum2 / 1000;
}

void ReadDirections(void){
  if(r1 - REST_P1 >  THRESH) d1 = RIGHT;
  if(r1 - REST_P1 < -THRESH) d1 = LEFT;

  if(r2 - REST_P2 >  THRESH) d2 = RIGHT;
  if(r2 - REST_P2 < -THRESH) d2 = LEFT;
}

int32_t LastGesture(void){
  while(1){
    ADC_ReadBoth(&r1, &r2);
    if(r1 - REST_P1 >  THRESH) return RIGHT;
    if(r1 - REST_P1 < -THRESH) return LEFT;
    if(r2 - REST_P2 >  THRESH) return RIGHT;
    if(r2 - REST_P2 < -THRESH) return LEFT;
  }
}

int32_t GetMatch(void){
  d1 = -1;
  d2 = -1;

  while(d1 == -1 || d2 == -1){
    ADC_ReadBoth(&r1, &r2);

    if(d1 == -1){
      if(r1 - REST_P1 >  THRESH) d1 = RIGHT;
      if(r1 - REST_P1 < -THRESH) d1 = LEFT;
    }

    if(d2 == -1){
      if(r2 - REST_P2 >  THRESH) d2 = RIGHT;
      if(r2 - REST_P2 < -THRESH) d2 = LEFT;
    }
  }

  return d1 != d2;
}
