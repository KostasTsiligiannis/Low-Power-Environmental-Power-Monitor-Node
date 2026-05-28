#include "uart.h"
#include "led.h"
#include "cli.h"
#include "scheduler.h"
#include "app.h"

char line_buffer[64];

int main(void)
{
    uart2_init();

    uart2_enable_rx_interrupt();

    led_init();

    uart2_write_string(
        "\r\n=================================\r\n"
        " STM32F303K8 Scheduler Starting\r\n"
        "=================================\r\n");

    uart2_write_string(
        "CLI Ready\r\n"
        "Type 'help' for commands\r\n");

    uint8_t line_idx = 0;

    while(1)
    {
        while(WRITE_IDX != READ_IDX)
        {
            char c = RINQ_BUFFER[READ_IDX];

            if(c == '\r' || c == '\n')
            {
                line_buffer[line_idx] = '\0';

                if(line_idx > 0)
                {
                    uart2_write_string("\r\n");

                    cli_process(line_buffer);
                }

                READ_IDX = (READ_IDX + 1) % 64;

                line_idx = 0;
            }
            else if(c == '\b' || c == 127)
            {
                if(line_idx > 0)
                {
                    line_idx--;
                }

                READ_IDX = (READ_IDX + 1) % 64;
            }
            else
            {
                if(line_idx < 63)
                {
                    line_buffer[line_idx++] = c;
                }

                READ_IDX = (READ_IDX + 1) % 64;
            }
        }
    }
}
