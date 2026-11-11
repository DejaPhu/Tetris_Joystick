/* Hardware-independent calibration and dominant-axis Schmitt trigger. */
#include <string.h>
#include "joystick_filter.h"
#include "joystick_config.h"
void joystick_filter_init(JoystickFilter *j) {
    memset(j,0,sizeof(*j));
    j->center_x=j->center_y=JOYSTICK_CENTER_DEFAULT;
    j->min_x=j->min_y=4095U;
}
uint8_t joystick_filter_step(JoystickFilter *j,uint16_t x,uint16_t y,uint8_t valid) {
    int32_t dx,dy,ax,ay;
    if (!valid || x>4095U || y>4095U) {
        if (!j->ready) joystick_filter_init(j);
        j->armed=0U;j->direction=0U;return 0U;
    }
    if (JOYSTICK_SWAP_XY) {uint16_t tmp=x;x=y;y=tmp;}
    if (!j->ready) {
        j->sum_x+=x;j->sum_y+=y;
        if(x<j->min_x)j->min_x=x;
        if(x>j->max_x)j->max_x=x;
        if(y<j->min_y)j->min_y=y;
        if(y>j->max_y)j->max_y=y;
        if(++j->samples<JOYSTICK_CAL_SAMPLES)return 0U;
        /* Never calibrate an off-center or moving stick as neutral. */
        if(j->min_x>=JOYSTICK_CENTER_MIN && j->max_x<=JOYSTICK_CENTER_MAX &&
           j->min_y>=JOYSTICK_CENTER_MIN && j->max_y<=JOYSTICK_CENTER_MAX &&
           (uint32_t)(j->max_x-j->min_x)<=JOYSTICK_CAL_SPREAD &&
           (uint32_t)(j->max_y-j->min_y)<=JOYSTICK_CAL_SPREAD) {
            j->center_x=(uint16_t)(j->sum_x/JOYSTICK_CAL_SAMPLES);
            j->center_y=(uint16_t)(j->sum_y/JOYSTICK_CAL_SAMPLES);
        }
        j->ready=1U;
    }
    dx=(int32_t)x-j->center_x;dy=(int32_t)y-j->center_y;
    if(JOYSTICK_INVERT_X)dx=-dx;
    if(JOYSTICK_INVERT_Y)dy=-dy;
    ax=dx<0?-dx:dx;ay=dy<0?-dy:dy;
    if(!j->armed) {
        if(ax<=JOYSTICK_RELEASE && ay<=JOYSTICK_RELEASE)j->armed=1U;
        return 0U;
    }
    /* Lock one axis until it returns inside the release band. A diagonal
     * cannot accidentally produce both move and rotate in the same sample. */
    if((j->direction==JOY_LEFT && dx < -JOYSTICK_RELEASE) ||
       (j->direction==JOY_RIGHT && dx > JOYSTICK_RELEASE) ||
       (j->direction==JOY_UP && dy < -JOYSTICK_RELEASE) ||
       (j->direction==JOY_DOWN && dy > JOYSTICK_RELEASE))return j->direction;
    j->direction=0U;
    if(ax>=JOYSTICK_ENTER && ax>=ay)j->direction=dx<0?JOY_LEFT:JOY_RIGHT;
    else if(ay>=JOYSTICK_ENTER)j->direction=dy<0?JOY_UP:JOY_DOWN;
    return j->direction;
}
