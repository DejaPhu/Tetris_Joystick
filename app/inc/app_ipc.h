#ifndef APP_IPC_H
#define APP_IPC_H
#include "cmsis_os2.h"
/* Defined and created ONLY in root main.c. No app_ipc_init/xQueueCreate here. */
extern osMessageQueueId_t xInputQueueHandle; /* 16 x uint16_t logical joystick/button codes */
extern osMessageQueueId_t xFrameQueueHandle; /* 1 x uint16_t notification */
#endif
