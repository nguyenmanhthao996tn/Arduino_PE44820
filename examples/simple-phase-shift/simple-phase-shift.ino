/**
 * @file simple-phase-shift.ino
 * @author nguyenmanhthao996tn (manhthao.spm@gmail.com)
 * @brief 
 * @version 1.0.0
 * @date 2022-05-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <PhaseShifter.h>

#define PIN_SI 28 // PD0
#define PIN_LE 29 // PD1
#define PIN_CLK 19 // PA3
#define DEVICE_ADDRESS 0x00

PhaseShifter pe44820(PIN_SI, PIN_LE, PIN_CLK, DEVICE_ADDRESS, MODE_OPT_1700_2200);

float foundAngle = 0;

void setup(void) {
  Serial.begin(9600);
  while (!Serial && (millis() < 5000)) {}

  pe44820.begin();

  Serial.println("Phase Shifter Controller Decode - EK44820");
}

void loop(void) {
  foundAngle = pe44820.setAngle(0);
  Serial.println("Target (degree): 0");
  Serial.print("Found (degree):");
  Serial.println(foundAngle);
  delay(3000);

  foundAngle = pe44820.setAngle(60);
  Serial.println("Target (degree): 60");
  Serial.print("Found (degree):");
  Serial.println(foundAngle);
  delay(3000);

  foundAngle = pe44820.setAngle(90);
  Serial.println("Target (degree): 90");
  Serial.print("Found (degree):");
  Serial.println(foundAngle);
  delay(3000);

  foundAngle = pe44820.setAngle(120);
  Serial.println("Target (degree): 120");
  Serial.print("Found (degree):");
  Serial.println(foundAngle);
  delay(3000);

  foundAngle = pe44820.setAngle(150);
  Serial.println("Target (degree): 150");
  Serial.print("Found (degree):");
  Serial.println(foundAngle);
  delay(3000);

  foundAngle = pe44820.setAngle(180);
  Serial.println("Target (degree): 180");
  Serial.print("Found (degree):");
  Serial.println(foundAngle);
  delay(3000);
}