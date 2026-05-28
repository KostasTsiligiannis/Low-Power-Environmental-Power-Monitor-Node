#include "app.h"
#include "scheduler.h"
#include "uart.h"
#include "led.h"

volatile uint8_t scheduler_enabled = 0;

void idle_task(void)
{
    while(1)
    {

    }
}

/*
 * TASK1
 * LED BLINK TASK
 */

void task1_handler(void)
{
    while(1)
    {
        led_on();

        uart2_write_string("TASK1 LED ON\r\n");

        task_delay(1000);

        led_off();

        uart2_write_string("TASK1 LED OFF\r\n");

        task_delay(1000);
    }
}

/*
 * UNUSED TASKS
 */

void task2_handler(void)
{
    while(1)
    {
        task_delay(1000);
    }
}

void task3_handler(void)
{
    while(1)
    {
        task_delay(1000);
    }
}

void task4_handler(void)
{
    while(1)
    {
        task_delay(1000);
    }
}
