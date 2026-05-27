#include <stdint.h>

/* ===== USART2 ===== */

#define USART2_BASE        0x40004400

#define USART2_CR1_ADDR    (USART2_BASE + 0x00)
#define USART2_BRR_ADDR    (USART2_BASE + 0x0C)
#define USART2_ISR_ADDR    (USART2_BASE + 0x1C)
#define USART2_RDR_ADDR    (USART2_BASE + 0x24)
#define USART2_TDR_ADDR    (USART2_BASE + 0x28)

#define USART_CR1_UE       (1U<<0)
#define USART_CR1_RE       (1U<<2)
#define USART_CR1_TE       (1U<<3)
#define USART_CR1_RXNEIE   (1U<<5)

#define USART_ISR_RXNE     (1U<<5)
#define USART_ISR_TXE      (1U<<7)

/* ===== NVIC ===== */

#define NVIC_ISER1_ADDR    0xE000E104
#define NVIC_USART2_IRQ    38U

/* Rx Buffers */
volatile uint8_t RINQ_BUFFER[64];
volatile uint8_t WRITE_IDX = 0;
volatile uint8_t READ_IDX = 0;


void uart2_init(void)
{
	//Section 1: Clock enable
	// RCC AHBENR
	uint32_t *pRccAhbEnr = (uint32_t*)0x40021014;

	// GPIOA MODER
	uint32_t *pGpioAMode = (uint32_t*)0x48000000;

	// GPIOA clock enable
	*pRccAhbEnr |= (1 << 17);

	// RCC APB1ENR
	uint32_t *pRccApb1Enr = (uint32_t*)0x4002101C;

	// USART2 clock enable
	*pRccApb1Enr |= (1 << 17);

	//Section 2:  GPIO alternate function configuration
	// PA2
	*pGpioAMode &= ~(3 << (2*2));
	*pGpioAMode |=  (2 << (2*2));

	// PA15 (Implemented a change from PA3 to PA15)
	*pGpioAMode &= ~(3 << (2*15));
	*pGpioAMode |=  (2 << (2*15));

	uint32_t *pGpioAAfrL = (uint32_t*)0x48000020;

	// PA2 -> Alternate Function
	*pGpioAAfrL &= ~(0xF << (4 * 2));
	*pGpioAAfrL |=  (7 << (4 * 2));

	// PA15 - AFRH (High register, pins 8-15), AF7
	uint32_t *pGpioAAfrH = (uint32_t*)0x48000024; // Address for AFRH

	*pGpioAAfrH &= ~(0xF << (4 * (15 - 8))); // Pin 15 is index 7 in AFRH
	*pGpioAAfrH |=  (7 << (4 * (15 - 8)));

	//SECTION 3 — USART peripheral setup
	//Cr1 - offset 0
	volatile uint32_t *pUsart2Cr1 = (volatile uint32_t*)USART2_CR1_ADDR;
	//Brr - offset 0x0C
	volatile uint32_t *pUsart2Brr = (volatile uint32_t*)USART2_BRR_ADDR;

	//Disable USART (UE bit) before BRR
	*pUsart2Cr1 &= ~USART_CR1_UE;
	// Set baudrate
	*pUsart2Brr = 0x341;
	//Reset value = 0, just set the two bits - RE & TE
	*pUsart2Cr1 |= USART_CR1_TE|USART_CR1_RE;
	//enable USART
	*pUsart2Cr1 |= USART_CR1_UE;
}

void uart2_write_char(char c){
    volatile uint32_t *pUsart2Isr = (volatile uint32_t*)USART2_ISR_ADDR;
    volatile uint32_t *pUsart2Tdr = (volatile uint32_t*)USART2_TDR_ADDR;

    // While TXE (bit 7) become 1 (Empty)
    while(((*pUsart2Isr) & (1U << 7)) == 0)
    {
    }

    *pUsart2Tdr = (uint32_t)c;
}


void uart2_write_string(char *str){

	int i=0;

	while(str[i] != '\0'){
		uart2_write_char(str[i]);
		i++;
	}

}

char uart2_read_char(void){
	volatile uint32_t *pUsart2Isr = (volatile uint32_t*)USART2_ISR_ADDR;
	volatile uint32_t *pUsart2Rdr = (volatile uint32_t*)USART2_RDR_ADDR;

    while(((*pUsart2Isr) & USART_ISR_RXNE) == 0)
    {
    }
	char c = *pUsart2Rdr;
	return c;
}

void uart2_enable_rx_interrupt(void){
	volatile uint32_t *pUsart2Cr1 = (volatile uint32_t*)USART2_CR1_ADDR;
	volatile uint32_t *pNvicIser1 = (volatile uint32_t*)NVIC_ISER1_ADDR;

    *pUsart2Cr1 |= USART_CR1_RXNEIE;   // RXNEIE

    *pNvicIser1 |= (1U<<(NVIC_USART2_IRQ-32U));   // USART2 IRQ enable
}


void USART2_EXTI26_IRQHandler(void)
{
    volatile uint32_t *pUsart2Isr = (volatile uint32_t*)USART2_ISR_ADDR;
    volatile uint32_t *pUsart2Rdr = (volatile uint32_t*)USART2_RDR_ADDR;

    if(*pUsart2Isr & USART_ISR_RXNE)
    {
        char c = (char)(*pUsart2Rdr);

        uint8_t next_write = (WRITE_IDX + 1) % 64;

        if(next_write != READ_IDX)
        {
            RINQ_BUFFER[WRITE_IDX] = c;
            WRITE_IDX = next_write;
        }
    }
}
