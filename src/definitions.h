
// Pinssss
#define sliderPin A0

#define batteryPin A3

#define airplaneBatteryDIOPin A6
#define airplaneBatteryClockPin A7

#define groundBatteryDIOPin 2
#define groundBatteryClockPin 3

#define signalDisplayDIOPin 4
#define signalDisplayClockPin 5

#define radioPin 7
#define radioPin2 8

// Objects
// Grove_LED_Bar signalDisplay(signalDisplayClockPin, signalDisplayDIOPin,
//                             false); // 220 mA full brightness
// TM1651 groundBatteryDisplay(groundBatteryClockPin, groundBatteryDIOPin);
// TM1651 airplaneBatteryDisplay(airplaneBatteryClockPin,
// airplaneBatteryDIOPin);

// Constants
#define delayTime 10
#define miniDelay 5
#define minAnalogReadFromBattery 750
#define maxAnalogReadFromBattery 1000

#define timeoutMilliSeconds 500

#define rollIndex 0
#define pitchIndex 1
#define yawIndex 2
#define throttleIndex 3

#define batteryLevelIndex 0



// void setupTimer()
// {
//   cli();
//   // set timer1 interrupt at 1Hz
//   TCCR1A = 0; // set entire TCCR1A register to 0
//   TCCR1B = 0; // same for TCCR1B
//   TCNT1 = 0;  // initialize counter value to 0
//   // set compare match register for 1hz increments
//   OCR1A = 15624; // = (16*10^6) / (1*1024) - 1 (must be <65536)
//   // turn on CTC mode
//   TCCR1B |= (1 << WGM12);
//   // Set CS12 and CS10 bits for 1024 prescaler
//   TCCR1B |= (1 << CS12) | (1 << CS10);
//   // enable timer compare interrupt
//   TIMSK1 |= (1 << OCIE1A);
//   sei();
// }

// ISR(TIMER1_COMPA_vect) {
//   // timer1 interrupt 1Hz toggles pin 13 (LED)
//   // generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for
//   full
//   // wave- toggle high then toggle low)
//   if (numberOfPackages > maxPackages)
//     maxPackages = numberOfPackages;

//   Serial.print("Max packages:\t");
//   Serial.println(maxPackages);

//   signalDisplay.setLevel(map(numberOfPackages, 0, maxPackages, 0, 10));

//   numberOfPackages = 0;
// }