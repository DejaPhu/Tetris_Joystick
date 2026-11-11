#ifndef TEST_CMSIS_OS2_H
#define TEST_CMSIS_OS2_H
#include <stdint.h>
typedef void *osMessageQueueId_t;
typedef enum { osOK,osErrorResource } osStatus_t;
osStatus_t osMessageQueuePut(osMessageQueueId_t queue,const void *item,uint8_t priority,uint32_t timeout);
#endif
