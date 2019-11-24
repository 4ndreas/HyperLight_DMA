/*
 * pin_config.h
 *
 *  Created on: 24.11.2019
 *      Author: ahoel
 */

#ifndef PIN_CONFIG_H_
#define PIN_CONFIG_H_

// D1 = Power
#define LED_BUILTIN_D2 PA6  // note LED overlaps with hardware SPI-1 ENC_MISO_PIN
#define LED_BUILTIN_D3 PA7  // note LED overlaps with hardware SPI-1 ENC_MOSI_PIN

// W5500 Ethernet
// HW-SPI-3 or HW_SPI-1
#define ETH_MOSI_PIN  PB5
#define ETH_MISO_PIN  PB4
#define ETH_SCK_PIN   PB3
#define ETH_SCS_PIN   PA15
#define ETH_INT_PIN   PB8
#define ETH_RST_PIN   PD3


/* UART2 To DMX IN/OUT	*/
#define UART2_TX_PIN  PD5	// TO MAX481 DI	- tx data in
#define UART2_RX_PIN  PD6	// TO MAX481 RO - rx data out
#define UART2_DE_PIN  PD7	// TO MAX481 DE - data direction
#define UART2_RE_PIN  PD4	// TO MAX481 RE - Recieve nEnable


/* HW-SPI-2 6-Pin Header	*/
/* +5V */
#define SPI2_CS_PIN    PB12
#define SPI2_SCK_PIN   PB13
#define SPI2_MISO_PIN  PC2
#define SPI2_MOSI_PIN  PC3
/* GND */


/* HW-SPI-3 6-Pin Header	*/
/* +5V */
#define SPI3_CS_PIN    PA4
#define SPI3_CLK_PIN   PA5
#define SPI3_MOSI_PIN  PA7
#define SPI3_MISO_PIN  PA6
/* GND */


/* UART1 4-Pin Header	*/
/* +5V */
#define UART1_RX_PIN  PA10
#define UART1_TX_PIN  PA9
/* GND */

/* UART3 4-Pin Header	*/
/* +5V */
#define UART3_RX_PIN  PD9
#define UART3_TX_PIN  PD8
/* GND */

/* I2C1 4-Pin Header	*/
/* +5V */
#define I2C1_SDA_PIN  PB7
#define I2C1_SCL_PIN  PB6
/* GND */

/* I2C2 4-Pin Header	*/
/* +5V */
#define I2C2_SDA_PIN  PB11
#define I2C2_SCL_PIN  PB10
/* GND */

/* Onbaord Flash		*/
// to do

/* SD-Card Pins  		*/
// to do

#endif /* PIN_CONFIG_H_ */
