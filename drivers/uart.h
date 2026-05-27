#ifndef UART_H_
#define UART_H_

#include <stdint.h>

/* UART API */

void uart2_init(void);
void uart2_write_char(char c);
void uart2_write_string(char *str);
char uart2_read_char(void);
void uart2_enable_rx_interrupt(void);

/* Ring Buffer globals */

extern volatile uint8_t RINQ_BUFFER[64];
extern volatile uint8_t WRITE_IDX;
extern volatile uint8_t READ_IDX;

#endif /* UART_H_ */
