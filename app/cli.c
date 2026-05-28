#include "cli.h"
#include "uart.h"
#include "led.h"
#include "scheduler.h"
#include <string.h>

typedef struct
{
    const char *name;
    void (*handler)(void);

}CLI_Command;

void cmd_help(void)
{
    uart2_write_string(
        "\r\nCommands:\r\n"
        "help\r\n"
        "status\r\n"
        "led on\r\n"
        "led off\r\n"
        "blink\r\n");
}

void cmd_status(void)
{
    uart2_write_string(
        "\r\nSYSTEM OK\r\n");
}

void cmd_led_on(void)
{
    led_on();

    uart2_write_string(
        "\r\nLED ON\r\n");
}

void cmd_led_off(void)
{
    led_off();

    uart2_write_string(
        "\r\nLED OFF\r\n");
}

void cmd_blink(void)
{
    uart2_write_string(
        "\r\nStarting Scheduler...\r\n");

    scheduler_init();

    while(1);
}

CLI_Command commands[] =
{
    {"help",cmd_help},
    {"status",cmd_status},
    {"led on",cmd_led_on},
    {"led off",cmd_led_off},
    {"blink",cmd_blink}
};

void cli_process(char *cmd)
{
    uint32_t n =
        sizeof(commands)/sizeof(commands[0]);

    for(uint32_t i=0;i<n;i++)
    {
        if(strcmp(cmd,commands[i].name)==0)
        {
            commands[i].handler();

            return;
        }
    }

    uart2_write_string(
        "\r\nUnknown command\r\n");
}
