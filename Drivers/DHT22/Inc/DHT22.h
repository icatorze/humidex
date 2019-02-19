/*
 * DHT22.h
 *
 *  Created on: 9 de fev de 2019
 *      Author: jack
 */

#ifndef DRIVERS_DHT22_INC_DHT22_H_
#define DRIVERS_DHT22_INC_DHT22_H_

/* Port and pin with DHT22 sensor*/
#define DHT22_GPIO_PORT			GPIOB
#define DHT22_GPIO_CLOCK		RCC_APB2Periph_GPIOB
#define DHT22_GPIO_PIN			GPIO_Pin_3

/* DHT22_GetReadings response codes */
#define DHT22_RCV_OK			0 // Return with no error
#define DHT22_RCV_NO_RESPONSE		1 // No response from sensor
#define DHT22_RCV_BAD_ACK1		2 // Bad first half length of ACK impulse
#define DHT22_RCV_BAD_ACK2		3 // Bad second half length of ACK impulse
#define DHT22_RCV_TIMEOUT		4 // Timeout while receiving bits
#define DHT22_BAD_DATA			5 // Bad data received

#include <stdint.h>

typedef struct dht22_data
{
  volatile uint8_t rcv_response;
  volatile float temperature;
  volatile float humidity;
  uint8_t parity;
  uint8_t parity_rcv;
  uint8_t hMSB;
  uint8_t hLSB;
  uint8_t tMSB;
  uint8_t tLSB;
  uint8_t bits[40];
} dht22_data;

void
DHT22_Init (void);

#include "types.h"
#include "main.h"

bool DHT22_Read (dht22_data *out);
void fmtTemp(float n, char o[9]);
void fmtHum(float n, char o[9]);
#endif /* DRIVERS_DHT22_INC_DHT22_H_ */
