/*
 */

#include "DHT22.h"
#include "stm32f0xx_hal_tim.h"
#include "tim.h"
//
//#include <stm32f10x_rcc.h>
//#include <stm32f10x_gpio.h>

volatile GPIO_PinState dhtpinstate;

volatile int count;

void
DHT22_Init (void)
{
//  RCC_APB2PeriphClockCmd (DHT22_GPIO_CLOCK, ENABLE);
//  PORT.GPIO_Pin = DHT22_Pin;
//  PORT.GPIO_Speed = GPIO_Speed_50MHz;
	HAL_TIM_Base_Start(&htim3);
}

static uint8_t
DHT22_GetReadings (dht22_data *out)
{
	uint8_t i;
	uint16_t c;

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	count = 0;

	// Switch pin to output
	GPIO_InitStruct.Pin = DHT22_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT22_GPIO_Port, &GPIO_InitStruct);

	// Generate start impulse
	HAL_GPIO_WritePin(DHT22_GPIO_Port,DHT22_Pin, GPIO_PIN_RESET);
	HAL_Delay(10); // Host start signal at least 1ms
	HAL_GPIO_WritePin(DHT22_GPIO_Port,DHT22_Pin, GPIO_PIN_SET);

	HAL_GPIO_DeInit(DHT22_GPIO_Port,DHT22_Pin);

	GPIO_InitStruct.Pin = DHT22_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DHT22_GPIO_Port, &GPIO_InitStruct);


	//  // Wait for AM2302 to begin communication (20-40us)
	__HAL_TIM_SET_COUNTER(&htim3,0);
	while (((count = __HAL_TIM_GET_COUNTER(&htim3))<100)
	  && (dhtpinstate=(HAL_GPIO_ReadPin(DHT22_GPIO_Port,DHT22_Pin))==GPIO_PIN_SET));
	count = __HAL_TIM_GET_COUNTER(&htim3);
	__HAL_TIM_SET_COUNTER(&htim3,0);
	if (count>=100) return DHT22_RCV_NO_RESPONSE;

    /* Waits more 80us until pin SET again */
	while (((count = __HAL_TIM_GET_COUNTER(&htim3))<100)
	  && (dhtpinstate=(HAL_GPIO_ReadPin(DHT22_GPIO_Port,DHT22_Pin))==GPIO_PIN_RESET));
    count = __HAL_TIM_GET_COUNTER(&htim3);
	__HAL_TIM_SET_COUNTER(&htim3,0);
	if ((count < 65) || (count > 95)) return DHT22_RCV_BAD_ACK1;

    /* Waits more 80us until pin RESET again */
	count = __HAL_TIM_GET_COUNTER(&htim3);
    while (((count = __HAL_TIM_GET_COUNTER(&htim3))<100)
	  && (dhtpinstate=(HAL_GPIO_ReadPin(DHT22_GPIO_Port,DHT22_Pin))==GPIO_PIN_SET));
	count = __HAL_TIM_GET_COUNTER(&htim3);
	__HAL_TIM_SET_COUNTER(&htim3,0);
	if ((count < 65) || (count > 95)) return DHT22_RCV_BAD_ACK2;


	i = 0;

	while (i < 40 ){
		/* Waits more 50us until pin RESET again for firt bit*/
		while (((count = __HAL_TIM_GET_COUNTER(&htim3))<100)
		  && (dhtpinstate=(HAL_GPIO_ReadPin(DHT22_GPIO_Port,DHT22_Pin))==GPIO_PIN_RESET));
	    count = __HAL_TIM_GET_COUNTER(&htim3);
		__HAL_TIM_SET_COUNTER(&htim3,0);
		if (count > 75) {
			while (((count = __HAL_TIM_GET_COUNTER(&htim3))<100)
			  && (dhtpinstate=(HAL_GPIO_ReadPin(DHT22_GPIO_Port,DHT22_Pin))==GPIO_PIN_SET));
			count = __HAL_TIM_GET_COUNTER(&htim3);
			__HAL_TIM_SET_COUNTER(&htim3,0);
			out->bits[i] = 0xff;
		}
		else {
			while (((count = __HAL_TIM_GET_COUNTER(&htim3))<100)
			  && (dhtpinstate=(HAL_GPIO_ReadPin(DHT22_GPIO_Port,DHT22_Pin))==GPIO_PIN_SET));
			count = __HAL_TIM_GET_COUNTER(&htim3);
			__HAL_TIM_SET_COUNTER(&htim3,0);

			out->bits[i] = (count < 100) ? (uint8_t) count : 0xff;
		}
		i++;
	}
	for (i = 0; i < 40; i++)
		if (out->bits[i] == 0xff)
			return DHT22_RCV_TIMEOUT;

  return DHT22_RCV_OK;
}

static void
DHT22_DecodeReadings (dht22_data *out)
{
  uint8_t i = 0;

  for (; i < 8; i++)
    {
      out->hMSB <<= 1;
      if (out->bits[i] > 48)
	out->hMSB |= 1;
    }

  for (; i < 16; i++)
    {
      out->hLSB <<= 1;
      if (out->bits[i] > 48)
	out->hLSB |= 1;
    }

  for (; i < 24; i++)
    {
      out->tMSB <<= 1;
      if (out->bits[i] > 48)
	out->tMSB |= 1;
    }

  for (; i < 32; i++)
    {
      out->tLSB <<= 1;
      if (out->bits[i] > 48)
	out->tLSB |= 1;
    }

  for (; i < 40; i++)
    {
      out->parity_rcv <<= 1;
      if (out->bits[i] > 48)
	out->parity_rcv |= 1;
    }

  out->parity = out->hMSB + out->hLSB + out->tMSB + out->tLSB;

}

static uint16_t
DHT22_GetHumidity (dht22_data *out)
{
  return (out->hMSB << 8) | out->hLSB;
}

static uint16_t
DHT22_GetTemperature (dht22_data *out)
{
  return (out->tMSB << 8) | out->tLSB;
}

bool
DHT22_Read (dht22_data *out)
{
  out->rcv_response = DHT22_GetReadings (out);
  if (out->rcv_response != DHT22_RCV_OK)
    {
      return false;
    }

  DHT22_DecodeReadings (out);

  if (out->parity != out->parity_rcv)
    {
      out->rcv_response = DHT22_BAD_DATA;
      return false;
    }

  out->humidity = (float) DHT22_GetHumidity (out) / 10.0f;

  uint16_t temperature = DHT22_GetTemperature (out);
  out->temperature = ((float) (temperature & 0x7fff)) / 10.0f;

  if (temperature & 0x8000)
    {
      out->temperature = -out->temperature;
    }

  return true;

}

void fmtTemp(float n, char o[9]){
	int p,f;
	p = (int)n;
	if (n>=p) {
		f = (int)((n-p)*100);
		sprintf(o,"T %2d.%02d%cC",p,f,223);
	}
	else {
		f = (int)((p-n)*100);
		sprintf(o,"T%+2d.%02d%cC",p,f,223);
	}


}

void fmtHum(float n, char o[9]){
	int p,f;
	p = (int)n;
	if (n>=p) f = (int)((n-p)*100);
	else f = (int)((p-n)*100);
	sprintf(o,"U %2d.%02d%% ",p,f);
}
