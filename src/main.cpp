#include "main.h"

void setup() {
  Serial.begin(115200);

#if !defined(__MIPSEL__)
  while (!Serial) {
    delay(100);  // Wait for serial port to connect - used on Leonardo, Teensy
                 // and other boards with built-in USB CDC serial connection
    Serial.println("Waiting serial ...");
  }
#endif

  // pinMode(LED_BUILTIN, OUTPUT);
  // digitalWrite(LED_BUILTIN, HIGH);  // Turn Of LED

  usbHostSetup();
  setupRadio();
}

void loop() {
  delay(delayTime);

  ps4();

  radioConnection();
}

void setupRadio() {
  printf_begin();

  while (!radio.begin()) {
    Serial.println("Radio hardware is not responding!");
    delay(200);
  }

  Serial.println("Radio is working");

  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(112);
  radio.setCRCLength(RF24_CRC_8);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  radio.setDataRate(RF24_250KBPS);
  radio.enableDynamicPayloads();

  radio.stopListening();

  radio.printDetails();
}

void usbHostSetup() {
  while (usb.Init()) {
    Serial.println("PS4 USB Host Failed");
    delay(200);
  }

  Serial.println("PS4 USB Library Started");
}

void printTransmissionData() {
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

void reset() {
  transmitData[rollIndex] = 90;
  transmitData[pitchIndex] = 90;
  transmitData[yawIndex] = 90;

  transmitData[autopilotIsOnIndex] = false;
}

void ps4() {
  usb.Task();

  transmitData[rollIndex] = map(PS4.getAnalogHat(LeftHatX), 0, 255, 0, 180);
  transmitData[pitchIndex] = map(PS4.getAnalogHat(RightHatY), 0, 255, 0, 180);
  transmitData[yawIndex] = map(PS4.getAnalogHat(RightHatX), 0, 255, 0, 180);

  L2Value = PS4.getAnalogButton(L2);
  R2Value = PS4.getAnalogButton(R2);

  if (PS4.getButtonClick(PS))
    Serial.print(F("\r\nPS"));

  if (PS4.getButtonClick(TRIANGLE))
    transmitData[autopilotIsOnIndex] = !transmitData[autopilotIsOnIndex];

  if (PS4.getButtonClick(CIRCLE))
    emergencyStopIsActive = !emergencyStopIsActive;

  if (PS4.getButtonClick(CROSS)) {
    emergencyStopIsActive = false;
    transmitData[autopilotIsOnIndex] = false;
    PS4AccelerometerEnabled = false;
  }

  if (PS4.getButtonClick(UP))
    trim += trimStep;

  if (PS4.getButtonClick(DOWN))
    trim -= trimStep;

  if (PS4.getButtonClick(OPTIONS))
    PS4AccelerometerEnabled = !PS4AccelerometerEnabled;

  // if (PS4.getButtonClick(RIGHT)) {
  //   Serial.print(F("\r\nRight"));
  // }

  // if (PS4.getButtonClick(LEFT)) {
  //   Serial.print(F("\r\nLeft"));
  // }

  // if (PS4.getButtonClick(L1))
  //   // Serial.print(F("\r\nL1"));
  //   digitalWrite(LED_BUILTIN, LOW);

  // if (PS4.getButtonClick(L3))
  //   Serial.print(F("\r\nL3"));

  // if (PS4.getButtonClick(R1))
  //   Serial.print(F("\r\nR1"));

  // if (PS4.getButtonClick(R3))
  //   Serial.print(F("\r\nR3"));

  // if (PS4.getButtonClick(SHARE))
  //   Serial.print(F("\r\nShare"));

  // if (PS4.getButtonClick(TOUCHPAD)) {
  //   Serial.print(F("\r\nTouchpad"));
  //   printTouch = !printTouch;
  // }

  if (PS4AccelerometerEnabled) {
    transmitData[rollIndex] = map(constrain(PS4.getAngle(Roll), 90, 270), 270, 90, 0, 180);
    transmitData[pitchIndex] = map(constrain(PS4.getAngle(Pitch), 90, 270), 270, 90, 0, 180);
  }

  // if (printTouch) {                              // Print the x, y coordinates of the touchpad
  //   if (PS4.isTouching(0) || PS4.isTouching(1))  // Print newline and carriage return if any of
  //   the
  //     // fingers are touching the touchpad
  //     Serial.print(F("\r\n"));
  //   for (uint8_t i = 0; i < 2; i++) {  // The touchpad track two fingers
  //     if (PS4.isTouching(i)) {         // Print the position of the finger if it
  //                                      // is touching the touchpad
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

void setLEDColor() {
  ColorsEnum ps4Color = Off;

  if (PS4AccelerometerEnabled)
    ps4Color = White;

  if (transmitData[autopilotIsOnIndex])
    ps4Color = Lightblue;

  if (emergencyStopIsActive)
    ps4Color = Red;

  if (millis() - lastRecievedTime >= timeoutMilliSeconds) {
    // PS4.setRumbleOn(RumbleHigh);
    ps4Color = Purple;
  } else if (millis() - lastRecievedTime >= timeoutMilliSeconds / 3) {
    // PS4.setRumbleOn(RumbleLow);
    ps4Color = Yellow;
  } else
    PS4.setRumbleOff();

  PS4.setLed(ps4Color);
}

void radioConnection() {
  transmitData[throttleIndex] =
      emergencyStopIsActive ? 0 : max(map(L2Value, 0, 255, 0, 90), map(R2Value, 0, 255, 0, 180));

  trim = constrain(trim, 90 - 45, 90 + 45);

  transmitData[trimIndex] = trim;

  radio.stopListening();

  if (radio.write(&transmitData, sizeof(transmitData)))
    radio.startListening();
  else
    Serial.println("Failed to transmit !");

  while (radio.available()) {
    radio.read(&recievedData, sizeof(recievedData));
    lastRecievedTime = millis();
  }

  setLEDColor();

  // Serial.print("Elapsed: ");
  // Serial.println(millis() - lastRecievedTime);
}

//  recievedData[rData.vibration] = (recievedData[rData.vibration] <
//  vibrationMinThreshold) ? 0 : recievedData[rData.vibration];

// float batteryValue = analogRead(batteryPin);

// groundBatteryDisplay.displayLevel(map(batteryValue,
// minAnalogReadFromBattery, maxAnalogReadFromBattery, 0, 7));

// pinMode(airplaneBatteryDIOPin, OUTPUT);
// pinMode(airplaneBatteryClockPin, OUTPUT);

// #if !defined(__MIPSEL__)
//   while (!Serial)
//     ;   // Wait for serial port to connect - used on Leonardo, Teensy and
//     other
//         // boards with built-in USB CDC serial connection
// #endif

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

// signalDisplay.begin();
// signalDisplay.setLevel(0);

// groundBatteryDisplay.init();
// airplaneBatteryDisplay.init();

// setupTimer();