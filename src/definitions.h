// #define batteryPin A3
// #define airplaneBatteryDIOPin A6
// #define airplaneBatteryClockPin A7
// #define groundBatteryDIOPin 2
// #define groundBatteryClockPin 3
// #define signalDisplayDIOPin 4
// #define signalDisplayClockPin 5

#include <Arduino.h>

#include <PS4USB.h>
#include "RF24.h"

// #include <PS4BT.h>
// #include "TM1651.h"
/*
 * 2    SDA
 * 3    SCL
 * 4    Radio CE
 * 5    Radio SCN
 * 6    Buzzer
 * 7    -
 * 8    -
 * 9    USB HOST INT
 * 10   USB HOST SS
 * 11   -
 * 12   -
 * 13   -
 */

#define sliderPin A0

// test
#define RadioChipEnabled 7
#define SCN 8

// leonardo
// #define RadioChipEnabled 4
// #define SCN 5
// #define BuzzerPin 6
// #define USBHostSS 10

// Objects
// Grove_LED_Bar signalDisplay(signalDisplayClockPin, signalDisplayDIOPin,
//                             false); // 220 mA full brightness
// TM1651 groundBatteryDisplay(groundBatteryClockPin, groundBatteryDIOPin);
// TM1651 airplaneBatteryDisplay(airplaneBatteryClockPin,
// airplaneBatteryDIOPin);

// Constants
#define delayTime 10
#define minAnalogReadFromBattery 750
#define maxAnalogReadFromBattery 1000

#define timeoutMilliSeconds 1000

#define rollIndex 0
#define pitchIndex 1
#define yawIndex 2
#define throttleIndex 3
#define autopilotIsOnIndex 4
#define pitchBiasIndex 5

#define batteryLevelIndex 0

USB usb;
// USBHub Hub1(&Usb); // Some dongles have a hub inside
// BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

PS4USB PS4(&usb);

bool printAngle, printTouch;

RF24 radio(RadioChipEnabled, SCN);

byte addresses[][6] = {"1Node", "2Node"};

byte transmitData[6];
byte recievedData[1];

byte L2Value = 0, R2Value = 0;

byte pitchBias = 90;
#define pitchBiasStep 2

unsigned long lastRecievedTime = millis();

bool emergencyStop = true;

void setupRadio();
void usbHostSetup();

void printTransmissionData();
void printRecievedData();
void reset();
void ps4();

void setLEDColor();

void radioConnection();

// int numberOfPackages = 0;
// int maxPackages = 0;
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
