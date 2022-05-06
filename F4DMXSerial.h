#ifndef F4DMXSERIAL_H
#define F4DMXSERIAL_H

#include "Arduino.h"
#include "variant.h"

/* UART2 To DMX IN/OUT	*/
#define UART2_TX_PIN  PD5	// TO MAX481 DI	- tx data in
#define UART2_RX_PIN  PD6	// TO MAX481 RO - rx data out
#define UART2_DE_PIN  PD7	// TO MAX481 DE - data direction
#define UART2_RE_PIN  PD4	// TO MAX481 RE - Recieve nEnable
// for RX RE LOW,  DE LOW
// for TX RE HIGH, DE HIGH


// ----- Constants -----
#define DMXSERIAL_MAX 512                  ///< The max. number of supported DMX data channels
#define DMX_BUFFER_SIZE (DMXSERIAL_MAX + 1)

// between 88us and 176us
#define DMXBREAK 92
// between 8us and < 1.0s
#define DMXMAB 12

#define DMX_GPIO_Port GPIOD
#define DMX_TX_GPIO_Pin  GPIO_PIN_5
#define DMX_RX_GPIO_Pin  GPIO_PIN_6

class F4DMXSerial
{

public:
    F4DMXSerial();
    void begin();

    void write(unsigned channel, uint8_t value);
    void write_block(unsigned first_channel, uint8_t* values, size_t len);
    void Send_Packet();

private:
    // buffer includes one byte more as start
    uint8_t dma_buffer_a[DMX_BUFFER_SIZE];
	uint8_t dma_buffer_b[DMX_BUFFER_SIZE];
	uint8_t* dma_front_buffer;
	uint8_t* dma_back_buffer;

    void setDataDir(uint8_t dir);
};

#endif
