// Library version 0.1b. See example sketch for information, or read down
// Deskwizard 03/10/2013

#include "DAC7678.h"
#include "Wire.h"
#include "Arduino.h"

unsigned char command;  // Command Byte
unsigned char msdb;     // Most Significant Data Byte
unsigned char lsdb;		// Least Significant Data Byte	
int address;		// DAC I2C address

DAC7678::DAC7678(unsigned char _address){
  address = _address;   // Set DAC to address passed to function
}

void DAC7678::init(){
  Wire.begin();   // Initialize I2C Bus
  reset(); 		  // Sends Power-On Reset command to DAC
}

void DAC7678::setVREF(bool _refstate){
  // Sets reference mode
  // _refmode 0: Static
  // _refmode 1: Flexible
  // _refstate 0: Internal vref turned off
  // _refstate 1: Internal reference always on
  unsigned char msdb;
  unsigned char lsdb;
  unsigned char command;

 // if(!_refmode){
    if (_refstate){
      command = 0x80; 
      msdb = 0x00;
      lsdb = 0x10;
    }
    else if (!_refstate){
      command = 0x80;
      msdb = 0x00;
      lsdb = 0x00;
    }
  // }
  // else if (_refmode){
    // if (_refstate){
      // command = 0x90; 
      // msdb = 0b01000000;
      // lsdb = 0x00;
    // }
    // else if (!_refstate){
      // command = 0x90;
      // msdb = 0b01100000;
      // lsdb = 0x00;
    // }
  // }
  transmit(command, msdb, lsdb); 
}


void DAC7678::reset(){
  // Issues Reset command similar to Power-On reset.
  transmit(0x70, 0x00, 0x00);
}

void DAC7678::enableChan(unsigned char channel, unsigned char state){
  // Sets Power-Down register for the specified DAC channel
  // a.k.a  Power-Down Register with specific channels bit set.
  // Enables or Disables (Powers up or down) the specific DAC output
  
  if (channel >= 0 || channel < 8){
     unsigned char _command = 0x40;
     unsigned int _value = 2 << channel;    
     unsigned char msdb = _value >> 4;

 if (state == 1){
    msdb |= 64;
  }
  if (state == 2){
    msdb |= 32;
  }
  if (state == 3){
    msdb |= 96;
  }
  unsigned char lsdb = _value << 4;
  transmit(_command, msdb, lsdb);
  }

}

void DAC7678::enableAll(unsigned char state){
  // Sets Power-Down register for all DAC channels
  // a.k.a  Power-Down Register with all channels bit set.
  // Enables or Disables all the channels at once

  unsigned char _command = 0x40;
  unsigned char msdb = 0x1F;
  if (state == 1){
    msdb |= 64;
  }
  if (state == 2){
    msdb |= 32;
  }
  if (state == 3){
    msdb |= 96;
  }
  unsigned char lsdb = 0xE0; 
  transmit(_command, msdb, lsdb);
}

void DAC7678::setAll(int _value){
  // Sets all DAC channels to specified value 
  // a.k.a. Individual Software LDAC Register with broadcast address
  // Will update and latch all channels
  
  // Check for out of range values
  if (_value >= 4096){
      _value = 4095; 
    }
  if (_value < 0){
      _value = 0; 
    }
  // Sets the variables
  unsigned char _command = 0x3F;   
  unsigned char msdb = _value >> 4;
  unsigned char lsdb = _value << 4;
  // Send data to DAC
  transmit(_command, msdb, lsdb);
}

void DAC7678::setChan(unsigned char channel, int _value){
  // Set specified channel (0-7) to the specified value
  // a.k.a. Individual Software LDAC Register with specific channel address
  // Will  update and latch specified channel
  
  //   Check for out of range values
  if (_value >= 4096){
      _value = 4095; 
    }
  if (_value < 0){
      _value = 0; 
    }

  if (channel >= 0 || channel < 8){ // Check for out of range Channel #
    // Sets the variables
    unsigned char _command = 0x30 + channel;
    unsigned char msdb = _value >> 4;
    unsigned char lsdb = _value << 4;
	// Send data to DAC
    transmit(_command, msdb, lsdb);
  }
}

void DAC7678::clrMode(int _value)
{
// Sets the DAC value to output on all channels when CLR pin is brought low
// a.k.a. Clear Code Register
// Will set the CLR Code register to output either zero, half-range, full range or to ignore the CLR pin
    unsigned char _command = 0x50;
    unsigned char msdb = 0x00;
    unsigned char lsdb = _value << 4;
	// Send data to DAC
    transmit(_command, msdb, lsdb);

}



void DAC7678::transmit(unsigned char _command, unsigned char _msdb, unsigned char _lsdb){
  // Transmit the actual command and high and low bytes to the DAC
  Wire.beginTransmission(address);
  Wire.write(_command);
  Wire.write(_msdb);
  Wire.write(_lsdb);
  Wire.endTransmission();

}