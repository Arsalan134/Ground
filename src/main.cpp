/*
 Example sketch for the PS4 USB library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */
#include "RF24.h"
#include <Arduino.h>
#include <PS4BT.h>
#include <PS4USB.h>
#include <usbhub.h>
 // #include "TM1651.h"
#include "printf.h"
#include <definitions.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
// USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the PS4BT class in two ways */
// This will start an inquiry and then pair with the PS4 controller - you only
// have to do this once You will need to hold down the PS and Share button at
// the same time, the PS4 controller will then start to blink rapidly indicating
// that it is in pairing mode
// PS4BT PS4(&Btd, PAIR);
// After that you can simply create the instance like so and then press the PS
// button on the device
// PS4BT PS4(&Btd);


PS4USB PS4(&Usb);


bool printAngle, printTouch;

RF24 radio(radioPin, radioPin2);

byte addresses[][6] = { "1Node", "2Node" };


byte transmitData[4];
byte recievedData[1];

byte oldL2Value = 0, oldR2Value = 0;

unsigned long timeoutMilliSeconds = 500;
unsigned long lastRecievedTime = millis();
unsigned long currentTime = millis();

bool emergencyStop = false;

void setup() {
  Serial.begin(115200);

  printf_begin();

  radio.begin();

  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(112);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  radio.stopListening();

  radio.printDetails();

  pinMode(airplaneBatteryDIOPin, OUTPUT);
  pinMode(airplaneBatteryClockPin, OUTPUT);

#if !defined(__MIPSEL__)
  while (!Serial)
    ;   // Wait for serial port to connect - used on Leonardo, Teensy and other
        // boards with built-in USB CDC serial connection
#endif

  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1)
      ; // Halt
  }

  Serial.print(F("\r\nPS4 Bluetooth Library Started\n"));

  // signalDisplay.begin();
  // signalDisplay.setLevel(0);

  // groundBatteryDisplay.init();
  // airplaneBatteryDisplay.init();

  // setupTimer();
}

void printTransmitData() {
  Serial.print("Send: ");
  for (unsigned long i = 0; i < sizeof(transmitData) / sizeof(transmitData[0]); i++) {
    Serial.print(transmitData[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void printRecievedData() {
  // Serial.print("Number Of Packages:\t");
  // Serial.println(numberOfPackages);

  Serial.print("Recieved: ");
  for (unsigned long i = 0; i < sizeof(recievedData) / sizeof(recievedData[0]); i++) {
    Serial.print(recievedData[i]);
    Serial.print(" ");
  }
  Serial.println();
}

// void signalDemo() {
//   for (int i = 0; i <= 10; i++) {
//     signalDisplay.setLed(i, 1);
//     delay(50);
//   }

//   for (int i = 10; i >= 0; i--) {
//     signalDisplay.setLed(i, 0);
//     delay(50);
//   }
// }

// void blink() {
//   for (int i = 0; i <= 7; i++) {
//     groundBatteryDisplay.displayLevel(i);
//     delay(50);
//     airplaneBatteryDisplay.displayLevel(i);
//   }

//   for (int i = 7; i >= 0; i--) {
//     groundBatteryDisplay.displayLevel(i);
//     delay(50);
//     airplaneBatteryDisplay.displayLevel(i);
//   }
// }

void reset() {
  transmitData[rollIndex] = 127;
  transmitData[pitchIndex] = 127;
  transmitData[yawIndex] = 127;
  transmitData[throttleIndex] = 0;

  recievedData[batteryLevelIndex] = 0;
}

// float map(float x, float in_min, float in_max, float out_min, float out_max)
// {
//   if (x >= in_max) return out_max;
//   if (x <= in_min) return out_min;
//   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
// }

// Recieves analog value from battery and shows it via displays
// void batteryLevelDemo(int value) {
//   int tis = value / 1000;
//   int sto = (value - tis * 1000) / 100;
//   int dec = (value - sto * 100) / 10;
//   int ed = (value - sto * 100) - 10 * dec;

//   airplaneBatteryDisplay.displayLevel(ed);
//   groundBatteryDisplay.displayLevel(dec);
//   signalDisplay.setLevel(value / 100);

//   Serial.println(value);
//   Serial.println(sto);
//   Serial.println(dec);
//   Serial.println(ed);
//   Serial.println();
// }

void ps4() {

  Usb.Task();

  if (PS4.connected()) {
    // if (PS4.getAnalogHat(LeftHatX) > 137 || PS4.getAnalogHat(LeftHatX) < 117
    // ||
    //     PS4.getAnalogHat(LeftHatY) > 137 || PS4.getAnalogHat(LeftHatY) < 117
    //     || PS4.getAnalogHat(RightHatX) > 137 || PS4.getAnalogHat(RightHatX) <
    //     117 || PS4.getAnalogHat(RightHatY) > 137 ||
    //     PS4.getAnalogHat(RightHatY) < 117) {
    //   Serial.print(F("\r\nLeftHatX: "));
    //   Serial.print(PS4.getAnalogHat(LeftHatX));
    //   Serial.print(F("\tLeftHatY: "));
    //   Serial.print(PS4.getAnalogHat(LeftHatY));
    //   Serial.print(F("\tRightHatX: "));
    //   Serial.print(PS4.getAnalogHat(RightHatX));
    //   Serial.print(F("\tRightHatY: "));
    //   Serial.print(PS4.getAnalogHat(RightHatY));
    // }

    if (PS4.getAnalogButton(L2) || PS4.getAnalogButton(R2)) { // These are the only analog buttons on the PS4 controller
      Serial.print(F("\r\nL2: "));
      Serial.print(PS4.getAnalogButton(L2));
      Serial.print(F("\tR2: "));
      Serial.print(PS4.getAnalogButton(R2));
    }

    if (PS4.getAnalogButton(L2) != oldL2Value || PS4.getAnalogButton(R2) != oldR2Value) // Only write value if it's different
      PS4.setRumbleOn(PS4.getAnalogButton(L2), PS4.getAnalogButton(R2));

    oldL2Value = PS4.getAnalogButton(L2);
    oldR2Value = PS4.getAnalogButton(R2);
    // Throttle

    // if (PS4.getButtonClick(PS))
    //   Serial.print(F("\r\nPS"));
    // if (PS4.getButtonClick(TRIANGLE)) {
    //   Serial.print(F("\r\nTriangle"));
    //   PS4.setRumbleOn(RumbleLow);
    // }
    // if (PS4.getButtonClick(CIRCLE)) {
    //   Serial.print(F("\r\nCircle"));
    //   PS4.setRumbleOn(RumbleHigh);
    // }
    // if (PS4.getButtonClick(CROSS)) {
    //   Serial.print(F("\r\nCross"));
    //   PS4.setLedFlash(10, 10); // Set it to blink rapidly
    // }
    if (PS4.getButtonClick(SQUARE)) {
      Serial.print(F("\r\nSquare"));
      // PS4.setLedFlash(0, 0); // Turn off blinking
      emergencyStop = true;
    }

    // if (PS4.getButtonClick(UP)) {
    //   Serial.print(F("\r\nUp"));
    //   PS4.setLed(Red);
    // }
    // if (PS4.getButtonClick(RIGHT)) {
    //   Serial.print(F("\r\nRight"));
    //   PS4.setLed(Blue);
    // }
    // if (PS4.getButtonClick(DOWN)) {
    //   Serial.print(F("\r\nDown"));
    //   PS4.setLed(Yellow);
    // }
    // if (PS4.getButtonClick(LEFT)) {
    //   Serial.print(F("\r\nLeft"));
    //   PS4.setLed(Green);
    // }

    // if (PS4.getButtonClick(L1))
    //   Serial.print(F("\r\nL1"));
    // if (PS4.getButtonClick(L3))
    //   Serial.print(F("\r\nL3"));
    // if (PS4.getButtonClick(R1))
    //   Serial.print(F("\r\nR1"));
    // if (PS4.getButtonClick(R3))
    //   Serial.print(F("\r\nR3"));

    // if (PS4.getButtonClick(SHARE))
    //   Serial.print(F("\r\nShare"));
    // if (PS4.getButtonClick(OPTIONS)) {
    //   Serial.print(F("\r\nOptions"));
    //   printAngle = !printAngle;
    // }
    // if (PS4.getButtonClick(TOUCHPAD)) {
    //   Serial.print(F("\r\nTouchpad"));
    //   printTouch = !printTouch;
    // }

    // if (printAngle) { // Print angle calculated using the accelerometer only
    //   Serial.print(F("\r\nPitch: "));
    //   Serial.print(PS4.getAngle(Pitch));
    //   Serial.print(F("\tRoll: "));
    //   Serial.print(PS4.getAngle(Roll));
    // }

    // if (printTouch) { // Print the x, y coordinates of the touchpad
    //   if (PS4.isTouching(0) ||
    //       PS4.isTouching(1)) // Print newline and carriage return if any of
    //       the
    //                          // fingers are touching the touchpad
    //     Serial.print(F("\r\n"));
    //   for (uint8_t i = 0; i < 2; i++) { // The touchpad track two fingers
    //     if (PS4.isTouching(i)) { // Print the position of the finger if it is
    //                              // touching the touchpad
    //       Serial.print(F("X"));
    //       Serial.print(i + 1);
    //       Serial.print(F(": "));
    //       Serial.print(PS4.getX(i));
    //       Serial.print(F("\tY"));
    //       Serial.print(i + 1);
    //       Serial.print(F(": "));
    //       Serial.print(PS4.getY(i));
    //       Serial.print(F("\t"));
    //     }
    //   }
    // }
  }
}

void radioConnection() {
  // signalDemo();
  // blink();  // 2 green, 2 red

  // float batteryValue = analogRead(batteryPin);

  // groundBatteryDisplay.displayLevel(map(batteryValue,
  // minAnalogReadFromBattery, maxAnalogReadFromBattery, 0, 7));

  // reset();
  reset();
  transmitData[rollIndex] = map(analogRead(xPin), 0, 1023, 0, 180);
  transmitData[pitchIndex] = map(analogRead(yPin), 0, 1023, 0, 180);

  // batteryLevelDemo(batteryValue);

  // Throttle
  if (!emergencyStop) {
    transmitData[rollIndex] = 0;
    transmitData[pitchIndex] = 0;
    transmitData[yawIndex] = 0;

    transmitData[throttleIndex] = max(oldR2Value, map(analogRead(sliderPin), 0, 1023, 0, 180));
  } else {
    transmitData[throttleIndex] = 0;
  }

  // Yaw
  // transmitData[tData.yaw] = map(analogRead(), 0, 255, 128, 255);

  //  recievedData[rData.vibration] = (recievedData[rData.vibration] <
  //  vibrationMinThreshold) ? 0 : recievedData[rData.vibration];

  //    emergencyStop = !emergencyStop;

  if (!radio.write(&transmitData, sizeof(transmitData))) {
    Serial.println("Failed to transmit 889");
  } else {
    // printTransmitData();
    delay(delayTime);
  }

  // currentTime = millis();

  // if (currentTime - lastRecievedTime > timeoutMilliSeconds) {
  //   Serial.println("TIMEOUT");
  // }

  // if (radio.available()) {
  //   radio.read(&recievedData, sizeof(recievedData));
  //   radio.stopListening();

  //   lastRecievedTime = millis();
  //   numberOfPackages++;
  //   printRecievedData();
  // }
}

void loop() {
  ps4();
  radioConnection();
}