#ifndef ACCEL_READ_H
#define ACCEL_READ_H

#include <stdint.h>

// Constants
#define LEFT   0
#define RIGHT  1
#define THRESH 700

// Global Variables (using 'extern' so they aren't redefined)
extern int32_t REST_P1, REST_P2;
extern int32_t r1, r2;
extern int32_t last_gesture;
extern int32_t match;
extern int8_t d1, d2;

// Function Prototypes
void ADC0_Init(void);
void ADC1_Init(void);
void ADC_ReadBoth(int32_t *p1, int32_t *p2);
void Calibrate(void);
void ReadDirections(void);
int32_t LastGesture(void);
int32_t GetMatch(void);

// Test Main Functions (if you still need them)
int main1S(void);
int main2S(void);

#endif