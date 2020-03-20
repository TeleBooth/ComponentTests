#include "stdint.h"
#include "stm32l4xx.h"
#include "stm32l476g_discovery.h"

#define MSEC		1
#define SEC			1000 * MSEC

#define MAX_TASK_DELAY	60 * SEC
#define MAX_WAIT		1 * SEC
#define MAX_TASKS		64

typedef enum task_type {
	NULL_TASK = -1, REQUEST_TASK = 0, RESPONSE_TASK = 1,
} TASK_TYPE;

typedef struct task_node {
	struct task_node *next;
	enum task_type task_type;
	uint8_t size;
	uint8_t task[256];
	uint32_t timeout;
	uint32_t repeat_interval;
} TASK_NODE;

void init_task_list(void);
uint8_t is_empty(void);
uint32_t bring_current(void);
void add_task(TASK_TYPE task_type, uint8_t size, void *task, uint32_t timeout,
		uint32_t repeat_interval);
void handle_tasks (UART_HandleTypeDef *huart);
