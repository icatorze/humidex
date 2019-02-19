# Humidex
Humidex based on STM32F0Discovery board and AM2302 Sensor.

The humidex (short for humidity index) is an index number used by Canadian meteorologists to describe how hot the weather feels to the average person, by combining the effect of heat and humidity. The term humidex is a Canadian innovation coined in 1965. The humidex is a dimensionless quantity based on the dew point. (Font: Wikipedia)

![equation](https://latex.codecogs.com/png.latex?\fn_jvn&space;\large&space;{\displaystyle&space;H=T_{\text{air}}&plus;0.5555\left[6.11e^{5417.7530\left({\frac&space;{1}{273.16}}-{\frac&space;{1}{273.15&plus;T_{\text{dew}}}}\right)}-10\right]})

- 20 to 29: Little to no discomfort
- 30 to 39: Some discomfort
- 40 to 45: Great discomfort; avoid exertion
- Above 45: Dangerous; heat stroke quite possible

![Humidex](https://github.com/icatorze/humidex/blob/master/Images/humidex1.png)


