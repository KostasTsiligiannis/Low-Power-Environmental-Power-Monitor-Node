#include "scheduler.h"
#include "app.h"

/* =========================
 * SCB Registers
 * ========================= */

#define ICSR        (*(volatile uint32_t*)0xE000ED04)

#define SYST_CSR    (*(volatile uint32_t*)0xE000E010)
#define SYST_RVR    (*(volatile uint32_t*)0xE000E014)

#define SHCSR       (*(volatile uint32_t*)0xE000ED24)

/* =========================
 * Globals
 * ========================= */

volatile uint32_t g_tick_count = 0;

TCB_t user_tasks[MAX_TASKS];

uint8_t current_task = 1;

/* =========================
 * Forward Declarations
 * ========================= */

void init_systick_timer(uint32_t tick_hz);

__attribute__((naked))
void init_scheduler_stack(uint32_t sched_top_of_stack);

void init_tasks_stack(void);

void enable_processor_faults(void);

__attribute__((naked))
void switch_sp_to_psp(void);

uint32_t get_psp_value(void);

void save_psp_value(uint32_t current_psp_value);

void update_next_task(void);

void unblock_tasks(void);

void update_global_tick_count(void);

/* =========================
 * Scheduler Init
 * ========================= */

void scheduler_init(void)
{
    enable_processor_faults();

    init_scheduler_stack(SCHED_STACK_START);

    init_tasks_stack();

    init_systick_timer(TICK_HZ);

    switch_sp_to_psp();

    task1_handler();
}

/* =========================
 * SysTick Init
 * ========================= */

void init_systick_timer(uint32_t tick_hz)
{
    uint32_t count_value =
            (SYSTICK_TIM_CLK / tick_hz) - 1;

    SYST_RVR &= ~(0x00FFFFFF);

    SYST_RVR |= count_value;

    /* Enable SysTick exception */
    SYST_CSR |= (1 << 1);

    /* Processor clock source */
    SYST_CSR |= (1 << 2);

    /* Enable SysTick */
    SYST_CSR |= (1 << 0);
}

/* =========================
 * Scheduler Stack Init
 * ========================= */

__attribute__((naked))
void init_scheduler_stack(uint32_t sched_top_of_stack)
{
    __asm volatile("MSR MSP,%0" : : "r"(sched_top_of_stack) : );
    __asm volatile("BX LR");
}

/* =========================
 * Task Stack Initialization
 * ========================= */

void init_tasks_stack(void)
{
    user_tasks[0].current_state = TASK_READY_STATE;
    user_tasks[1].current_state = TASK_READY_STATE;
    user_tasks[2].current_state = TASK_READY_STATE;
    user_tasks[3].current_state = TASK_READY_STATE;
    user_tasks[4].current_state = TASK_READY_STATE;

    user_tasks[0].psp_value = IDLE_STACK_START;
    user_tasks[1].psp_value = T1_STACK_START;
    user_tasks[2].psp_value = T2_STACK_START;
    user_tasks[3].psp_value = T3_STACK_START;
    user_tasks[4].psp_value = T4_STACK_START;

    user_tasks[0].task_handler = idle_task;
    user_tasks[1].task_handler = task1_handler;
    user_tasks[2].task_handler = task2_handler;
    user_tasks[3].task_handler = task3_handler;
    user_tasks[4].task_handler = task4_handler;

    uint32_t *pPSP;

    for(int i = 0 ; i < MAX_TASKS ; i++)
    {
        /* 8-byte alignment */
        user_tasks[i].psp_value &= ~0x7;

        pPSP = (uint32_t*) user_tasks[i].psp_value;

        /* xPSR */
        pPSP--;
        *pPSP = DUMMY_XPSR;

        /* PC */
        pPSP--;
        *pPSP = ((uint32_t)user_tasks[i].task_handler | 0x1);

        /* LR */
        pPSP--;
        *pPSP = 0xFFFFFFFD;

        /* R12, R3, R2, R1, R0, R11-R4 */
        for(int j = 0 ; j < 13 ; j++)
        {
            pPSP--;
            *pPSP = 0;
        }

        user_tasks[i].psp_value = (uint32_t)pPSP;
    }
}

/* =========================
 * Fault Enable
 * ========================= */

void enable_processor_faults(void)
{
    SHCSR |= (1 << 16); /* MemManage */
    SHCSR |= (1 << 17); /* BusFault */
    SHCSR |= (1 << 18); /* UsageFault */
}

/* =========================
 * PSP Management
 * ========================= */

uint32_t get_psp_value(void)
{
    return user_tasks[current_task].psp_value;
}

void save_psp_value(uint32_t current_psp_value)
{
    user_tasks[current_task].psp_value =
            current_psp_value;
}

/* =========================
 * Task Selection
 * ========================= */

void update_next_task(void)
{
    int state = TASK_BLOCKED_STATE;

    for(int i = 0 ; i < MAX_TASKS ; i++)
    {
        current_task++;

        current_task %= MAX_TASKS;

        state = user_tasks[current_task].current_state;

        if((state == TASK_READY_STATE) &&
           (current_task != 0))
        {
            break;
        }
    }

    if(state != TASK_READY_STATE)
    {
        current_task = 0;
    }
}

/* =========================
 * Switch MSP -> PSP
 * ========================= */

__attribute__((naked))
void switch_sp_to_psp(void)
{
    __asm volatile ("PUSH {LR}");

    __asm volatile ("BL get_psp_value");

    __asm volatile ("MSR PSP,R0");

    __asm volatile ("POP {LR}");

    __asm volatile ("MOV R0,#0x02");

    __asm volatile ("MSR CONTROL,R0");

    __asm volatile ("ISB");

    __asm volatile ("BX LR");
}

/* =========================
 * Trigger Context Switch
 * ========================= */

void schedule(void)
{
    /* Pend PendSV exception */
    ICSR |= (1 << 28);
}

/* =========================
 * Task Delay
 * ========================= */

void task_delay(uint32_t tick_count)
{
    INTERRUPT_DISABLE();

    if(current_task)
    {
        user_tasks[current_task].block_count =
                g_tick_count + tick_count;

        user_tasks[current_task].current_state =
                TASK_BLOCKED_STATE;

        schedule();
    }

    INTERRUPT_ENABLE();
}

/* =========================
 * PendSV Handler
 * ========================= */

__attribute__((naked))
void PendSV_Handler(void)
{
    /* Save context */

    __asm volatile("MRS R0,PSP");

    __asm volatile("STMDB R0!,{R4-R11}");

    __asm volatile("PUSH {LR}");

    __asm volatile("BL save_psp_value");

    /* Select next task */

    __asm volatile("BL update_next_task");

    /* Restore next task context */

    __asm volatile("BL get_psp_value");

    __asm volatile("LDMIA R0!,{R4-R11}");

    __asm volatile("MSR PSP,R0");

    __asm volatile("ISB");

    __asm volatile("POP {LR}");

    __asm volatile("BX LR");
}

/* =========================
 * Tick Management
 * ========================= */

void update_global_tick_count(void)
{
    g_tick_count++;
}

void unblock_tasks(void)
{
    for(int i = 1 ; i < MAX_TASKS ; i++)
    {
        if(user_tasks[i].current_state != TASK_READY_STATE)
        {
            if(user_tasks[i].block_count ==
                    g_tick_count)
            {
                user_tasks[i].current_state =
                        TASK_READY_STATE;
            }
        }
    }
}

/* =========================
 * SysTick Handler
 * ========================= */

void SysTick_Handler(void)
{
    update_global_tick_count();

    unblock_tasks();

    /* Trigger PendSV */
    ICSR |= (1 << 28);
}

/* =========================
 * Fault Handlers
 * ========================= */

void HardFault_Handler(void)
{
    while(1);
}

void MemManage_Handler(void)
{
    while(1);
}

void BusFault_Handler(void)
{
    while(1);
}
