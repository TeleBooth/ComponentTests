#include "stdint.h"
#include "string.h"
#include "task_list.h"
#include "stm32l4xx.h"
//#include "stm32l476g_discovery.h"

static TASK_NODE tasks[MAX_TASKS];
static TASK_NODE *head;
static TASK_NODE *tail;
static uint8_t num_tasks;
static uint32_t last_time;

static void handle_response (UART_HandleTypeDef *huart, TASK_NODE *t);
static void handle_request (UART_HandleTypeDef *huart, TASK_NODE *t);

void init_task_list() {
	uint8_t cnt;
	head = tasks;
	tail = tasks;
	TASK_NODE *temp = head;
	for (cnt = 1; cnt < MAX_TASKS; cnt++) {
		temp->task_type = NULL_TASK;
		temp->next = &tasks[cnt];
		temp = temp->next;
	}
	temp->task_type = NULL_TASK;
	temp->next = head;
	last_time = HAL_GetTick();
	num_tasks = 0;
}

uint8_t is_empty() {
	if (num_tasks) {
		return 1;
	} else {
		return 0;
	}
}

uint32_t bring_current() {
	uint32_t dt;
	uint32_t current_time = HAL_GetTick();
	if (current_time < last_time) {
		dt = (UINT32_MAX - last_time) + current_time;
	} else {
		dt = current_time - last_time;
	}
	last_time = current_time;
	if (num_tasks == 0) {
		return 0xFFFFFFFF;
	} else {
		if (head->timeout < dt) {
			head->timeout = 0;
			return 0;
		} else {
			head->timeout = head->timeout - dt;
			return (head->timeout > MAX_WAIT) ? MAX_WAIT : head->timeout;
		}
	}
}

void add_task(TASK_TYPE task_type, uint8_t size, void *task, uint32_t timeout,
		uint32_t repeat_interval) {
	if (num_tasks == MAX_TASKS) {
		return;
	}
	uint8_t cnt;
	TASK_NODE *t, *temp;
	// If empty simply fill head with relevant data
	if (num_tasks == 0) {
		t = head;
		t->task_type = task_type;
		t->size = size;
		memcpy(t->task, (uint8_t *) task, size);
		t->timeout = timeout;
		t->repeat_interval = repeat_interval;
		num_tasks++;
		return;
	}
	// If not empty take a free task (tail->next) and fill it with relevant data (except timeout, which is relative to the other tasks)
	t = tail->next;
	t->task_type = task_type;
	t->size = size;
	memcpy(t->task, (uint8_t *) task, size);
	t->repeat_interval = repeat_interval;
	// Unlink the task from the list
	tail->next = t->next;
	t->next = NULL;
	// Process the case were the task has to be inserted at the beginning of the task list
	if (timeout < head->timeout) {
		// Get empty task that precedes the head in the circular task list
		for (cnt = 0, temp = head; cnt < MAX_TASKS - 2; cnt++, temp = temp->next);
		temp->next = t;
		t->timeout = timeout;
		t->next = head;
		head->timeout = head->timeout - timeout;
		head = t;
		num_tasks++;
		return;
	}
	// Iterate through task list and find where the task should be inserted based on its timeout.
	for (cnt = 0, temp = head; cnt < num_tasks; cnt++, temp = temp->next) {
		timeout = timeout - temp->timeout;
		// Handle case when we are at the end of the task list
		if (cnt == num_tasks - 1) {
			t->timeout = timeout;
			t->next = temp->next;
			temp->next = t;
			tail = t;
			break;
		}
		// Handle normal case
		else if (timeout < temp->next->timeout) {
			t->timeout = timeout;
			temp->next->timeout = temp->next->timeout - timeout;
			t->next = temp->next;
			temp->next = t;
			break;
		}
	}
	num_tasks++;
}

void handle_tasks (UART_HandleTypeDef *huart) {
	if (bring_current() > 2) {
		return;
	}
	TASK_NODE *t = head;
	head = head->next;
	num_tasks--;
	switch (t->task_type) {
		case RESPONSE_TASK:
			handle_response(huart, t);
			break;
		case REQUEST_TASK:
			handle_request(huart, t);
			break;
		default:
			break;
	}
	if (t->repeat_interval) {
		bring_current();
		add_task(t->task_type, t->size, t->task, t->repeat_interval, t->repeat_interval);
	}
	t->task_type = NULL_TASK;
	return;
}

void handle_response (UART_HandleTypeDef *huart, TASK_NODE *t) {

}

void handle_request (UART_HandleTypeDef *huart, TASK_NODE *t) {
	HAL_UART_Transmit_DMA(huart, &t->size, 1);
	while(HAL_UART_Transmit_DMA(huart, t->task, t->size) == HAL_BUSY);
}
