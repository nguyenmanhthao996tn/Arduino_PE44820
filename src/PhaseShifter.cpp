/**
 * @file PhaseShifter.cpp
 * @author nguyenmanhthao996tn (manhthao.spm@gmail.com)
 * @brief A compact Arduino library to control digital phase shifter PE44820
 * @version 1.0.0
 * @date 2022-05-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "PhaseShifter.h"
#include "mapping.h"

PhaseShifter::PhaseShifter(void)
{
  this->_data.raw = 0;
  this->_address = 0x00;
  this->_mode = MODE_BINARY_WEIGHTED;

  this->_si_pin = -1;
  this->_le_pin = -1;
  this->_clk_pin = -1;
}

PhaseShifter::PhaseShifter(int si_pin, int le_pin, int clk_pin, uint8_t address, mode_e mode)
{
  this->_data.raw = 0;
  this->_address = address;
  this->_mode = mode;

  this->_si_pin = si_pin;
  this->_le_pin = le_pin;
  this->_clk_pin = clk_pin;

  begin();
}

void PhaseShifter::begin(void)
{
  /* GPIOs Initialize */
  pinMode(this->_si_pin, OUTPUT);
  digitalWrite(this->_si_pin, LOW);

  pinMode(this->_le_pin, OUTPUT);
  digitalWrite(this->_le_pin, HIGH);

  pinMode(this->_clk_pin, OUTPUT);
  digitalWrite(this->_clk_pin, HIGH);
}

void PhaseShifter::dataShiftOut(uint8_t addr, data_u data)
{
  int8_t bit_index;

  digitalWrite(this->_le_pin, LOW);
  _PS_delay_100ns(1); // Tsettle > 10ns

  // Data
  for (bit_index = 0; bit_index < 8; bit_index++)
  {
    digitalWrite(this->_clk_pin, HIGH);
    digitalWrite(this->_si_pin, (((data.data.data >> bit_index) & 0x01) != 0) ? HIGH : LOW);
    _PS_delay_100ns(1); // Tclkh > 30ns

    digitalWrite(this->_clk_pin, LOW);
    _PS_delay_100ns(1); // Tclkl > 30ns
  }

  // OPT
  digitalWrite(this->_clk_pin, HIGH);
  digitalWrite(this->_si_pin, (data.data.opt != 0) ? HIGH : LOW);
  _PS_delay_100ns(1); // Tclkh > 30ns

  digitalWrite(this->_clk_pin, LOW);
  _PS_delay_100ns(1); // Tclkl > 30ns

  // Address
  for (bit_index = 0; bit_index < 4; bit_index++)
  {
    digitalWrite(this->_clk_pin, HIGH);
    digitalWrite(this->_si_pin, (((addr >> bit_index) & 0x01) != 0) ? HIGH : LOW);
    _PS_delay_100ns(1); // Tclkh > 30ns

    digitalWrite(this->_clk_pin, LOW);
    _PS_delay_100ns(1); // Tclkl > 30ns
  }

  // Reset SI & SLK Pin
  digitalWrite(this->_si_pin, LOW);
  digitalWrite(this->_clk_pin, HIGH);

  _PS_delay_100ns(1); // Tsettle > 10ns
  digitalWrite(this->_le_pin, HIGH);
}

uint16_t PhaseShifter::findAngle(float angle, mode_e mode, float *foundAngle)
{
  uint16_t idx = 0;
  int16_t targetAngle = angle * 10;

  for (idx = 0; idx <= 255; idx++)
  {
    if (targetAngle <= PHASE_SHIFT_VALUE[idx])
    {
      break;
    }
  }

  if ((idx != 0) && (idx != 255))
  {
    uint16_t value1 = abs(targetAngle - PHASE_SHIFT_VALUE[idx - 1]);
    uint16_t value2 = abs(targetAngle - PHASE_SHIFT_VALUE[idx]);

    if (value1 < value2)
    {
      idx -= 1;
    }
  }
  if (foundAngle != NULL)
  {
    (*foundAngle) = PHASE_SHIFT_VALUE[idx] / 10.0;
  }

  uint16_t retVal = 0;
  switch (mode)
  {
  case MODE_BINARY_WEIGHTED:
    retVal = BINARY_WEIGHTED_DATA[idx];
    break;
  case MODE_OPT_1700_2200:
    retVal = OPT_1700_2200_DATA[idx];
    break;
  case MODE_OPT_2600:
    retVal = OPT_2600_DATA[idx];
    break;
  default:
    retVal = 0x0000;
    break;
  }

  return retVal;
}

float PhaseShifter::setAngle(float angle)
{
  float foundAngle = 0;

  if (angle < 0)
    angle = 0;
  if (angle > 360)
    angle = 360;

  this->_data.raw = findAngle(angle, this->_mode, &foundAngle);
  this->dataShiftOut(this->_address, this->_data);

  return foundAngle;
}

void PhaseShifter::setData(data_u data)
{
  this->_data.raw = data.raw;
}

data_u PhaseShifter::getData(void)
{
  return (this->_data);
}

void PhaseShifter::setAddress(uint8_t address)
{
  this->_address = address & 0x0F;
}

uint8_t PhaseShifter::getAddress(void)
{
  return (this->_address);
}

void PhaseShifter::setMode(mode_e mode)
{
  if ((mode != MODE_BINARY_WEIGHTED) && (mode != MODE_OPT_1700_2200) && (mode != MODE_OPT_2600))
  {
    return; // Invalid mode parameter
  }

  this->_mode = mode;
}

mode_e PhaseShifter::getMode(void)
{
  return (this->_mode);
}

void PhaseShifter::setPins(int si_pin, int le_pin, int clk_pin)
{
  this->_si_pin = si_pin;
  this->_le_pin = le_pin;
  this->_clk_pin = clk_pin;
}

void PhaseShifter::getPins(int *si_pin, int *le_pin, int *clk_pin)
{
  if (si_pin != NULL)
  {
    (*si_pin) = this->_si_pin;
  }

  if (le_pin != NULL)
  {
    (*le_pin) = this->_le_pin;
  }

  if (clk_pin != NULL)
  {
    (*clk_pin) = this->_clk_pin;
  }
}
