# mbed-multifunction-worldclock-C-

C++ code for MBED OS, turning a FRDM-K64F board with the Mbed Application Shield into a clock that can be used as a stopwatch, alarm clock, timer, or displaying time across different time zones. 

Init:	Shows the time and “Press Fire to set time”
Set Time:	The left pot sets hours and the right pot sets minutes. Fire confirms
Current Time	Shows: HH:MM:SS
World Time:	The left pot picks one of 25 cities (GMT−11 to GMT+12, including Yangon at +6.5). It shows local time next to Manchester time
Stopwatch	Fire: starts, stops, and resets-and-restarts. The LED is blue
Countdown:	The pots set MM:SS and Fire starts it. The green LED blinks while it counts. At zero, any screen jumps to “Countdown has finished!” and the speaker buzzes at about 1 kHz until you press Fire
