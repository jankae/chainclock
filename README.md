# chainclock

The chainclock is a very inaccurate clock which displays the time by means of rotating a chain.

<p align="left">
  <img src="/images/IMG_2279.JPG?size=200" width="350"/>
</p>

The chain-loop consists of 120 links, with a number attached to every 10th link. Since the driving gear has ten teeth, this results in one rotation per hour. After some failed experiments with attaching the gear to the hour hand of a normal clock (it was either too loud, too weak or both), I opted for a 28BYJ-48 stepper motor.

This also called for a microcontroller (ATtiny2313) with the simpliest of programs. To save energy, the motor doesn't continually move. Instead it is only activated whenever a minute has passed. The microcontroller performs a few steps, checks the voltage and goes back to sleep.

The clock is powered by a 3,7V Lipo battery and stops completely if the battery runs low (there is no further indicator, the clock simply stops until the battery is replaced).

## Construction
The central part is the stepper motor (28BYJ-48). Some additional aluminium, a ball bearing to lighten the load on the motor (should be fine even without it) and a teardrop wooden enclosure completes the clock.

## Compiling & Flashing
1. See PCB-files for the pinout of the ISP connector. 
2. Adjust the makefile to your programmer
3. Setting fuses:

  ```
  cd software
  make fuses
  ```
4. Flash the program:

  ```
  make flash
  ```
  ## Operation
  Attach the chain and sit back ;)
