#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"

extern "C" {
  void __disable_irq(void);
  void __enable_irq(void);
}

#define LEFT     0
#define RIGHT    1
#define UP       2
#define DOWN     3
#define NO_PUNCH -1
#define THRESH   200

// Calibration baselines
uint32_t REST_P1X, REST_P1Y;
uint32_t REST_P2X, REST_P2Y;

// Global direction state
volatile int8_t g_dir1 = NO_PUNCH;
volatile int8_t g_dir2 = NO_PUNCH;

// ── PLL ────────────────────────────────────────────────────────────────────
void PLL_Init(void) {
  Clock_Init80MHz(0);
}

// ── ADC Init ───────────────────────────────────────────────────────────────
// ADC0: P1 X on ch6 (MEMCTL[0]), P1 Y on ch7 (MEMCTL[1])
// ADC1: P2 X on ch5 (MEMCTL[0]), P2 Y on ch4 (MEMCTL[1])
void ADC0_Init(void) {
  ADC0->ULLMEM.GPRCM.RSTCTL = 0xB1000003;
  ADC0->ULLMEM.GPRCM.PWREN  = 0x26000001;
  Clock_Delay(24);
  ADC0->ULLMEM.GPRCM.CLKCFG = 0xA9000000;
  ADC0->ULLMEM.CLKFREQ       = 7;
  ADC0->ULLMEM.CTL0          = 0x03010000;
  ADC0->ULLMEM.CTL1          = 0x00000000;
  ADC0->ULLMEM.CTL2          = 0x00000000;
  ADC0->ULLMEM.MEMCTL[0]     = 6;   // P1 X
  ADC0->ULLMEM.MEMCTL[1]     = 7;   // P1 Y
  ADC0->ULLMEM.SCOMP0        = 0;
  ADC0->ULLMEM.CPU_INT.IMASK = 0;
}

void ADC1_Init(void) {
  ADC1->ULLMEM.GPRCM.RSTCTL = 0xB1000003;
  ADC1->ULLMEM.GPRCM.PWREN  = 0x26000001;
  Clock_Delay(24);
  ADC1->ULLMEM.GPRCM.CLKCFG = 0xA9000000;
  ADC1->ULLMEM.CLKFREQ       = 7;
  ADC1->ULLMEM.CTL0          = 0x03010000;
  ADC1->ULLMEM.CTL1          = 0x00000000;
  ADC1->ULLMEM.CTL2          = 0x00000000;
  ADC1->ULLMEM.MEMCTL[0]     = 5;   // P2 X
  ADC1->ULLMEM.MEMCTL[1]     = 4;   // P2 Y
  ADC1->ULLMEM.SCOMP0        = 0;
  ADC1->ULLMEM.CPU_INT.IMASK = 0;
}

// ── ADC Read ───────────────────────────────────────────────────────────────
// Triggers a 2-conversion sequence on each ADC and reads both results
void ADC_ReadAll(uint32_t *p1x, uint32_t *p1y,
                 uint32_t *p2x, uint32_t *p2y) {
  // Set sequence end on MEMCTL[1] for both ADCs
  ADC0->ULLMEM.MEMCTL[1] |= 0x00010000;
  ADC1->ULLMEM.MEMCTL[1] |= 0x00010000;

  // Enable and trigger both ADCs
  ADC0->ULLMEM.CTL0 |= 0x1;
  ADC0->ULLMEM.CTL1  = (ADC0->ULLMEM.CTL1 & ~0x00000100) | 0x00000100;
  ADC1->ULLMEM.CTL0 |= 0x1;
  ADC1->ULLMEM.CTL1  = (ADC1->ULLMEM.CTL1 & ~0x00000100) | 0x00000100;

  // Wait for both to finish
  while((ADC0->ULLMEM.STATUS & 0x01) == 0x01){}
  while((ADC1->ULLMEM.STATUS & 0x01) == 0x01){}

  *p1x = ADC0->ULLMEM.MEMRES[0];
  *p1y = ADC0->ULLMEM.MEMRES[1];
  *p2x = ADC1->ULLMEM.MEMRES[0];
  *p2y = ADC1->ULLMEM.MEMRES[1];
}

// ── Calibration ────────────────────────────────────────────────────────────
void Calibrate(void) {
  uint32_t p1x, p1y, p2x, p2y;
  uint64_t s1x = 0, s1y = 0, s2x = 0, s2y = 0;

  // Signal: hold still
  LaunchPad_LED1_On();
  Clock_Delay1ms(2000);

  for(uint32_t i = 0; i < 1000; i++) {
    ADC_ReadAll(&p1x, &p1y, &p2x, &p2y);
    s1x += p1x;  s1y += p1y;
    s2x += p2x;  s2y += p2y;
  }

  REST_P1X = s1x / 1000;
  REST_P1Y = s1y / 1000;
  REST_P2X = s2x / 1000;
  REST_P2Y = s2y / 1000;

  // Signal: done
  LaunchPad_LED1_Off();
  LaunchPad_LED2_On();
  Clock_Delay1ms(500);
  LaunchPad_LED2_Off();
}

// ── Direction classify ─────────────────────────────────────────────────────
int8_t classify_dir(int32_t dx, int32_t dy) {
  int32_t ax = dx < 0 ? -dx : dx;
  int32_t ay = dy < 0 ? -dy : dy;
  if(ax < THRESH && ay < THRESH) return NO_PUNCH;
  if(ax >= ay) return dx > 0 ? RIGHT : LEFT;
  else         return dy > 0 ? UP    : DOWN;
}

// ── get_input (call at 30 Hz) ──────────────────────────────────────────────
void get_input(void) {
  uint32_t p1x, p1y, p2x, p2y;
  ADC_ReadAll(&p1x, &p1y, &p2x, &p2y);

  int32_t d1x = (int32_t)p1x - (int32_t)REST_P1X;
  int32_t d1y = (int32_t)p1y - (int32_t)REST_P1Y;
  int32_t d2x = (int32_t)p2x - (int32_t)REST_P2X;
  int32_t d2y = (int32_t)p2y - (int32_t)REST_P2Y;

  g_dir1 = classify_dir(d1x, d1y);
  g_dir2 = classify_dir(d2x, d2y);
}

// ── get_match ──────────────────────────────────────────────────────────────
// Returns shared direction 0-3, or -1 if mismatch / no punch
int8_t get_match(void) {
  if(g_dir1 == NO_PUNCH || g_dir2 == NO_PUNCH) return NO_PUNCH;
  if(g_dir1 != g_dir2)                          return NO_PUNCH;
  return g_dir1;
}

// ── Direction label helper ─────────────────────────────────────────────────
const char* dir_name(int8_t d) {
  switch(d) {
    case LEFT:  return "LEFT";
    case RIGHT: return "RIGHT";
    case UP:    return "UP";
    case DOWN:  return "DOWN";
    default:    return "---";
  }
}

// ── Main ───────────────────────────────────────────────────────────────────
int main(void) {
  __disable_irq();
  PLL_Init();
  LaunchPad_Init();
  ADC0_Init();
  ADC1_Init();

  // UART assumed initialized by LaunchPad_Init or add your UART init here
  printf("Calibrating... hold hands still\r\n");
  Calibrate();
  printf("Calibration done.\r\n");
  printf("REST P1: X=%lu  Y=%lu\r\n", REST_P1X, REST_P1Y);
  printf("REST P2: X=%lu  Y=%lu\r\n", REST_P2X, REST_P2Y);

  __enable_irq();

  uint32_t tick = 0;

  while(1) {
    // 30 Hz: ~33ms at 80MHz with this busy delay
    Clock_Delay1ms(33);

    get_input();

    int8_t match = get_match();

    // Print raw ADC deltas and classified directions every tick
    uint32_t p1x, p1y, p2x, p2y;
    ADC_ReadAll(&p1x, &p1y, &p2x, &p2y);

    int32_t d1x = (int32_t)p1x - (int32_t)REST_P1X;
    int32_t d1y = (int32_t)p1y - (int32_t)REST_P1Y;
    int32_t d2x = (int32_t)p2x - (int32_t)REST_P2X;
    int32_t d2y = (int32_t)p2y - (int32_t)REST_P2Y;

    printf("[%lu] P1 dx=%5ld dy=%5ld -> %-5s | P2 dx=%5ld dy=%5ld -> %-5s | match=%s\r\n",
      tick,
      (long)d1x, (long)d1y, dir_name(g_dir1),
      (long)d2x, (long)d2y, dir_name(g_dir2),
      match == NO_PUNCH ? "---" : dir_name(match)
    );

    tick++;
  }
}