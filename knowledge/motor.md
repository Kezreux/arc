
# Information regarding the electric motor running the RC car

The motor is the heart of the RC car - choosing the 'wrong' motor could be catastrophic.

This entire RC car project is intended to be a learning project when it comes to embedded systems, circuit design, PCBs, power electronics and programming. My main goal being optimizing the motor drive, getting a smooth and powerful RC car running, all other functionality is just a bonus.

The plan is to buy two different motors, first one to be a bench motor where I am play around with and optimize the motor controller and STM32s firmware. The second motor is the one going in the car.

## Bench motor
Hobbywing QuicRun 3650 G2, 13.5T

## Car motor
Hobbywing EZRUN 3665SD G3, 3200 kV

## Concept
These motors are three-phase systems meaning that we need to create a three-phase inverter to feed the motor. This is where the sensors on the motors come in. Each Hall sensor will tell our control module where the motors current position is in, and our firmware will then fire the next inverter sequence. The faster the motor spins, the faster the control module needs to fire the inverter.
