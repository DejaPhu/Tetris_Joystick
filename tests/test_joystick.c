#include <assert.h>
#include <stdio.h>
#include "joystick_filter.h"
#include "button_filter.h"
static void center(JoystickFilter *j,uint16_t x,uint16_t y) {
 unsigned i;joystick_filter_init(j);
 for(i=0;i<32;i++)assert(joystick_filter_step(j,x,y,1)==0);
}
int main(void) {
 JoystickFilter j;ButtonFilter b={0};unsigned i;uint8_t d;unsigned presses=0,repeats=0;
 center(&j,2100,2000);assert(j.ready && j.armed && j.center_x==2100 && j.center_y==2000);
 for(i=0;i<1000;i++)assert(!joystick_filter_step(&j,2090+i%21,1990+i%21,1));
 assert(joystick_filter_step(&j,1401,2000,1)==0);
 assert(joystick_filter_step(&j,1400,2000,1)==JOY_LEFT);
 assert(joystick_filter_step(&j,1650,2000,1)==JOY_LEFT);
 assert(joystick_filter_step(&j,1700,2000,1)==0);
 assert(joystick_filter_step(&j,2800,2000,1)==JOY_RIGHT);
 assert(joystick_filter_step(&j,2100,2000,1)==0);
 assert(joystick_filter_step(&j,2100,1300,1)==JOY_UP);
 assert(joystick_filter_step(&j,0,0,1)==JOY_UP); /* locks one diagonal axis */
 assert(joystick_filter_step(&j,2100,2000,1)==0);
 assert(joystick_filter_step(&j,2100,2700,1)==JOY_DOWN);
 assert(joystick_filter_step(&j,2100,2000,1)==0);
 assert(joystick_filter_step(&j,3000,2900,1)==JOY_RIGHT);
 assert(joystick_filter_step(&j,2100,2000,1)==0);
 assert(joystick_filter_step(&j,3100,2800,1)==JOY_RIGHT);
 assert(!joystick_filter_step(&j,3100,2800,0));assert(!j.armed);
 assert(!joystick_filter_step(&j,3100,2000,1));
 assert(!joystick_filter_step(&j,2100,2000,1));assert(j.armed);
 assert(joystick_filter_step(&j,3100,2000,1)==JOY_RIGHT);
 center(&j,0,4095);assert(j.ready && !j.armed && j.center_x==2048 && j.center_y==2048);
 assert(!joystick_filter_step(&j,0,4095,1));
 assert(!joystick_filter_step(&j,2048,2048,1));assert(j.armed);
 /* Moving calibration falls back to the nominal center. */
 joystick_filter_init(&j);
 for(i=0;i<32;i++)joystick_filter_step(&j,i%2?1800:2400,2048,1);
 assert(j.center_x==2048 && j.ready);
 joystick_filter_init(&j);joystick_filter_step(&j,2048,2048,1);
 joystick_filter_step(&j,2048,2048,0);assert(j.samples==0);
 assert(!joystick_filter_step(&j,5000,2048,1));assert(!j.ready);
 /* Full analog-to-debounce path: UP emits one press, horizontal hold repeats. */
 center(&j,2048,2048);
 for(i=0;i<=1000;i+=10) {
  d=joystick_filter_step(&j,2048,0,1);
  if(button_filter_step(&b,d==JOY_UP,0,i)==1)presses++;
 }
 assert(presses==1);
 b=(ButtonFilter){0};center(&j,2048,2048);
 for(i=0;i<=530;i+=10) {
  d=joystick_filter_step(&j,4095,2048,1);
  switch(button_filter_step(&b,d==JOY_RIGHT,1,i)) {case 1:presses++;break;case 2:repeats++;break;}
 }
 assert(presses==2 && repeats==3);
 assert(!button_filter_step(&b,0,1,630));
 puts("PASS: neutral calibration, startup deflection, noise, thresholds/hysteresis, four directions, diagonal lock, ADC recovery, rotate once, hold repeat");
 return 0;
}
