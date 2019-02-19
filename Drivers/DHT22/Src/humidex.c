#include "humidex.h"
#include "math.h"
#include "stdio.h"

float humidex(float temperature, float humidity) {
	float kelvin, eTs, eTd, hx;
	kelvin = temperature + 273;
	eTs = pow(10,((-2937.4 /kelvin)-4.9283* log(kelvin)/log(10) +23.5471));
	eTd = eTs * humidity /100;
	hx = round(temperature + ((eTd-10)*5/9));
	return hx;
}

void fmtHmdxL1(float hmdx, char o[6]){
	if (hmdx<=29) sprintf(o,"Otimo ");
	else if ((hmdx>29)&&(hmdx<=39)) sprintf(o," Bom  ");
	else if ((hmdx>39)&&(hmdx<=49)) sprintf(o," Ruim ");
	else if ((hmdx>49)&&(hmdx<=59)) sprintf(o,"Pessim");
	else if (hmdx>59) sprintf(o," Fodeu");
	else sprintf(o,"------");
}
