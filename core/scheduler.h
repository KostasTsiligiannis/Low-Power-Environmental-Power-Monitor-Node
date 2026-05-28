#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>

#define MAX_TASKS              5

#define TASK_READY_STATE       0x00
#define TASK_BLOCKED_STATE     0xFF

#define DUMMY_XPSR             0x01000000U

#define TICK_HZ                1000U

#define SIZE_TASK_STACK        256U
#define SIZE_SCHED_STACK       256U

#define SRAM_START             0x20000000U
#define SIZE_SRAM              (9*1024)

#define SRAM_END               (SRAM_START + SIZE_SRAM)

#define T1_STACK_START         SRAM_END
#define T2_STACK_START         (SRAM_END-(1*SIZE_TASK_STACK))
#define T3_STACK_START         (SRAM_END-(2*SIZE_TASK_STACK))
#define T4_STACK_START         (SRAM_END-(3*SIZE_TASK_STACK))
#define IDLE_STACK_START       (SRAM_END-(4*SIZE_TASK_STACK))
#define SCHED_STACK_START      (SRAM_END-(5*SIZE_TASK_STACK))

#define HSI_CLOCK              16000000U
#define SYSTICK_TIM_CLK        HSI_CLOCK

#define INTERRUPT_DISABLE() \
do{ \
__asm volatile("MOV R0,#0x1"); \
__asm volatile("MSR PRIMASK,R0"); \
}while(0)

#define INTERRUPT_ENABLE() \
do{ \
__asm volatile("MOV R0,#0x0"); \
__asm volatile("MSR PRIMASK,R0"); \
}while(0)

typedef struct
{
    uint32_t psp_value;

    uint32_t block_count;

    uint8_t current_state;

    void (*task_handler)(void);

}TCB_t;

extern volatile uint32_t g_tick_count;

void scheduler_init(void);

void task_delay(uint32_t tick_count);

void schedule(void);

#endif
