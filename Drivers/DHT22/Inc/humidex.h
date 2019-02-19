/*
 */

#ifndef DRIVERS_DHT22_INC_HUMIDEX_H_
#define DRIVERS_DHT22_INC_HUMIDEX_H_

float humidex(float temperature, float humidity);
void fmtHmdxL1(float hmdx, char o[6]);

#endif /* DRIVERS_DHT22_INC_HUMIDEX_H_ */
