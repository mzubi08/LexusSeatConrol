# LexusSeatConrol
### Devices used:
- Arduino R4 w/ screw terminal hat.
- Generic 3 Phase DC Brushless Motor Controller, such as this one: https://a.co/d/24UN6mX
- 100K Digital Potentiometer, such as this one: https://a.co/d/4Uf68bQ
- 5v Relay Board
- Generic themistor (for the seatback, the only built-in thermistor was in the seat bottom)
- 2 resistors for wiring the the thermistors (value TBA)
- Terminal blocks: 2 for 12v power buses, 1 for a 5v power bus, 1 for connecting seat components to the arduino (makes wiring easier).
- 14 guage wire (overkill, but seat heaters draw a lot of power).
- 120v ac to 12v dc power supply for powering electronics.
- Not used but should have been: 5V power supply for the Arduino, relays, and thermistors.
- Solid core ethernet wiring. It's nicely rigid and pairs are twitsted together, which is convenient. I used pairs taken from a Cat5 cable for various 5v wiring. Does not solder well though.
- Cadillac Deville Seat Heat/Cool control. 

### Notes:
Currently the thermistors and relay board run off the Arduino's supplied 5v (and it is supplied by the 12v dc [NOT ALL ARDUINOS CAN ACCEPT 12v IN]), but for greater stability they should have a seperate 5v supply. Powering them off the arduino means that when the arduino is powered via a USB for debugging, it cannot supply enough 5v power for things to operate properly. 
