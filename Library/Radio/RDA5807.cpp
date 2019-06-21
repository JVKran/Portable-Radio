#include "hwlib.hpp"
#include "RDA5807.hpp"

RDA5807::RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address, int bandLimit, unsigned int channelSpacing):
	Radio(bus, address, bandLimit),
	indexAddress(address + 1),
	channelSpacing(channelSpacing)
{
	setData();
}


//Done
void RDA5807::setData(){
	auto transaction = bus.write(address); 	//0x10 for sequential access
	for(unsigned int i = 2; i < 11; i++){
		transaction.write(data[i]);
	}
	hwlib::wait_ms(30);
}

void RDA5807::setData(const int regNumber){
	auto transaction = bus.write(address + 1); 	//0x11 for random access
	transaction.write(regNumber);
	transaction.write(static_cast<uint8_t>((data[regNumber] & 0xFF00) >> 8));
	transaction.write(static_cast<uint8_t>(data[regNumber] & 0x00FF));
	hwlib::wait_ms(30);
}

void RDA5807::setRegister(const int regNumber, const uint16_t value){
	data[regNumber] = value;
	auto transaction = bus.write(address + 1); 	//0x11 for random access
	transaction.write(regNumber);
	transaction.write(static_cast<uint8_t>((value & 0xFF00) >> 8));
	transaction.write(static_cast<uint8_t>(value & 0x00FF));
	hwlib::wait_ms(30);
}

int RDA5807::getStatus(const int regNumber){
	bus.write(address + 1).write(regNumber + 10);
	auto transaction = bus.read(address + 1);
	auto b1 = transaction.read_byte();
	auto b2 = transaction.read_byte();
	status[regNumber] = ( b1 << 8 ) | ( b2 );
	return ( b1 << 8 ) | ( b2 );
}

//Done
unsigned int RDA5807::signalStrength(){
	getStatus(1);
	return (status[1] >> 8);
}

//Done
void RDA5807::setMute(const bool mute){
	if(mute){
		data[0] &= ~(1UL << 6);
	} else {
		data[0] |= (1UL << 6);
	}
	setData();
}

void RDA5807::setBandLimit(const unsigned int limit){
	if(limit < 4){
		data[3] |= (limit << 2);
	} else {
		data[3] |= (3UL << 2);
	}
	shortData[0] = 0x02;
	shortData[1] = 0xC0;
	shortData[2] = 0x03;
	bus.write(0x11).write(shortData, 3);
	hwlib::wait_ms(200);
	shortData[0] = 0x02;
	shortData[1] = 0xC0;
	shortData[2] = 0x0D;
	bus.write(0x11).write(shortData, 3);
}

unsigned int RDA5807::hasBandLimit(){
	return int((data[3] >> 2) & 1) + int((data[3] >> 3));
}

//Done
void RDA5807::setStereo(const bool stereo){
	if(stereo){
		data[0] &= ~(1UL << 5);
	} else {
		data[0] |= (1UL << 5);
	}
	setData();
}

//Done
bool RDA5807::stereoReception(){
	return (status[0] >> 2) & 1;
}

//Done
void RDA5807::setSpacing(const unsigned int spacing){
	if(spacing == 25000){
		data[3] |= 3UL;
	} else if (spacing == 50000){
		data[3] |= (1UL << 1);
		data[3] &= ~1UL;
	} else if (spacing == 200000){
		data[3] |= 1UL;
		data[3] &= ~(1UL << 1);
	} else {
		data[3] &= ~3UL;
	}
	setData();
}

//Done???		https://www.mikrocontroller.net/topic/313562#3411429
void RDA5807::setFrequency(int frequency){
	

}

void RDA5807::setVolume(unsigned int volume){
	data[5] &= ~(15);
	if(volume <= 15){
		data[5] |= volume;
	}
	setData();
}

void RDA5807::tune(const bool tune){
	if(tune){
		data[3] |= (1UL << 4);
	} else {
		data[3] &= ~(1UL << 4);
	}
	setData(3);
}

void RDA5807::standBy(const bool standby){
	if(standby){
		data[2] &= ~1UL;
	} else {
		data[2] |= 1UL;
	}
	setData(2);
}

//Done
void RDA5807::normalOutput(const bool normal){
	if(normal){
		data[0] |= (1UL << 7);
	} else {
		data[0] &= ~(1UL << 7);
	}
}

//Done
void RDA5807::setBassBoost(const bool boost){
	if(boost){
		data[0] |= (1UL << 4);
	} else {
		data[0] &= ~(1UL << 4);
	}
}

void RDA5807::init(){

   auto freq = 892;                            // 89.2 MHz
   auto freqB = freq - 870;
   auto freqH = freqB>>2;
   auto freqL = (freqB&3)<<6;                  // Shift channel selection for matching register 0x03


          // Device address 0x11 (random access)
   shortData[0] = (0x02);                      // Register address 0x02
   shortData[1] = (0xC0); 
   shortData[2] = (0x00);    // write 0xC002 into Reg.3 (soft reset, enable)                         // wait 500ms
   bus.write(0x11).write(shortData, 3);

   hwlib::wait_ms(1000);
   shortData[0] = (0x02);                      // Register address 0x02
   shortData[1] = (0xC0); 
   shortData[2] = (0x0D);    // write 0xC002 into Reg.3 (soft reset, enable)                         // wait 500ms
   bus.write(0x11).write(shortData, 3); // write 0xC00D (RDS on etc.) into 0x02
  
   hwlib::wait_ms(1000);
   shortData[0] = (0x03);                      // Register address 0x02
   shortData[1] = (freqH); 
   shortData[2] = (freqL + 0x10);    // write 0xC002 into Reg.3 (soft reset, enable)                         // wait 500ms
   bus.write(0x11).write(shortData, 3);

	/*
	/// Reset and Enable Powerup
	data[2] |= 3UL;
	data[4] = 0x1800;
	data[5] = 0x9081;
	data[6] = 0x0000;
	data[7] = 0x0000;
	data[8] = 0x0000;
	data[9] = 0x0000;
	//Reset
	setData();
	//Enable
	setRegister(2, 1);
	*/
}