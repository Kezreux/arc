
# Inverter

Inverting means to change the voltage and current type from DC to AC - in our instance we need to create a three phase inverter. 
Used to control the motor forward and backwards, together with the motor this form the entire motor and is the heart of this project. 

## Generating three phases
The general idea of three phase inverting is to have six 'switches' one can control, be it either a transistor, MOSFET, IGBT or more. The workings of an inverter is to control when to turn the DC voltage on or off - creating the sinusoidal curve, which is 360 degrees per cycle.

The way these six switches work is in a half bridge fashion, meaning that two switches in 'series', one on each side of the load, can be on at a time.
