#include "uart.h"
#include "led.h"
#include "cli.h"

char line_buffer[64];

int main(void)
{
    uart2_init();
    uart2_enable_rx_interrupt();
    led_init();
    uart2_write_string("UART READY. Type a word and press ENTER:\r\n");

    // Μετακίνηση των μεταβλητών εδώ για να έχουν scope σε όλη τη main
    uint8_t line_idx = 0;

    while(1)
    {
        while(WRITE_IDX != READ_IDX)
        {
            char c = RINQ_BUFFER[READ_IDX];

            if(c == '\r' || c == '\n')
            {
                // Προσθήκη null terminator στο τέλος της γραμμής
                line_buffer[line_idx] = '\0';

                // Εκτύπωση και επεξεργασία μόνο αν υπάρχει κείμενο
                if(line_idx > 0)
                {
                    uart2_write_string("\r\nECHO: ");
                    uart2_write_string(line_buffer);
                    uart2_write_string("\r\n");

                    cli_process(line_buffer);
                }

                // Ενημέρωση του pointer και μηδενισμός του counter για την επόμενη γραμμή
                READ_IDX = (READ_IDX + 1) % 64;
                line_idx = 0;
            }
            else if(c == '\b' || c == 127) // Backspace
            {
                if(line_idx > 0)
                {
                    line_idx--;
                }

                READ_IDX = (READ_IDX + 1) % 64;
            }
            else // Κανονικός χαρακτήρας
            {
                if(line_idx < 63)
                {
                    line_buffer[line_idx] = c;
                    line_idx++;
                }

                READ_IDX = (READ_IDX + 1) % 64;
            }
        }
    }
}
