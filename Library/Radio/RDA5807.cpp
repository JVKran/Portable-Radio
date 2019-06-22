#include "hwlib.hpp"
#include "RDA5807.hpp"

RDA5807::RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address, int bandLimit, float channelSpacing):
	Radio(bus, address, bandLimit),
	indexAddress(address + 1),		//0x10 for sequential access and 0x11 for indexed access
	channelSpacing(channelSpacing)
{}

void RDA5807::setData(){
	auto transaction = bus.write(address);
	for(unsigned int i = 2; i < 8; i++){
		transaction.write(data[i]);
	}
	hwlib::wait_ms(30);
}

void RDA5807::setData(const int regNumber){
	auto transaction = bus.write(indexAddress);
	transaction.write(regNumber);
	transaction.write(static_cast<uint8_t>((data[regNumber] & 0xFF00) >> 8));
	transaction.write(static_cast<uint8_t>(data[regNumber] & 0x00FF));
	hwlib::wait_ms(30);
}

void RDA5807::setRegister(const int regNumber, const uint16_t value){
	data[regNumber] = value;
	auto transaction = bus.write(indexAddress);
	transaction.write(regNumber);
	transaction.write(static_cast<uint8_t>((value & 0xFF00) >> 8));
	transaction.write(static_cast<uint8_t>(value & 0x00FF));
	hwlib::wait_ms(30);
}

void RDA5807::getStatus(const int regNumber){
	bus.write(indexAddress).write(regNumber);
	auto transaction = bus.read(indexAddress);
	auto b1 = transaction.read_byte();
	auto b2 = transaction.read_byte();
	status[regNumber] = ( b1 << 8 ) | ( b2 );
}

void RDA5807::getStatus(){
	auto transaction = bus.read(address);
	for(unsigned int i = 0; i < 6; i++){
		auto b1 = transaction.read_byte();
      	auto b2 = transaction.read_byte();
		status[i] = ( b1 << 8 ) | ( b2 );
	}
}

void RDA5807::begin(){
	data[2] |= (1UL << 15);
	data[2] |= (1UL << 14);
	setData(2);
	hwlib::wait_ms(500);
	powerUpEnable(true);
}

//Done
void RDA5807::setClockSupply(const bool clockSource, const bool directInput){
	if(clockSource){
		data[2] |= (1UL << 11);
	} else {
		data[2] &= ~(1UL << 11);
	}
	if(directInput){
		data[2] |= (1UL << 10);
	} else {
		data[2] &= ~(1UL << 10);
	}
	setData(2);
}

//Done
void RDA5807::setClockFrequency(const unsigned int frequency){
	data[2] &= ~(7UL << 4); 	//Clear all bits first (also equal to 32768kHz)
	switch(frequency){
		case 12:
			data[2] |= (1UL << 4);
			break;
		case 13:
			data[2] |= (2UL << 4);
			break;
		case 19:
			data[2] |= (3UL << 4);
			break;
		case 24:
			data[2] |= (5UL << 4);
			break;
		case 26:
			data[2] |= (6UL << 4);
			break;
		case 38:
			data[2] |= (7UL << 4);
	}
	setData(2);
}

//Done
void RDA5807::demodulateMethod(const bool newMethod){
	if(newMethod){
		data[2] |= (1UL << 2);
	} else {
		data[2] &= ~(1UL << 2);
	}
	setData(2);
}

//Done
void RDA5807::reset(){
	data[2] |= (1UL << 1);
	setData(2);
	//Might have to unset again?
}

void RDA5807::powerUpEnable(const bool enable){
	if(enable){
		data[2] |= 1UL;
	} else {
		data[2] &= ~1UL;
	}
	setData(2);
}

unsigned int RDA5807::signalStrength(){
	getStatus(1);
	return (status[1] >> 8);
}

//Done
void RDA5807::setMute(const bool mute){
	if(mute){
		data[2] &= ~(1UL << 14);
	} else {
		data[2] |= (1UL << 14);
	}
	setData(2);
}

//Done
void RDA5807::normalAudio(const bool normal){
	if(normal){
		data[2] |= (1UL << 15);
	} else {
		data[2] &= ~(1UL << 15);
	}
	setData(2);
}

//Done
void RDA5807::setStereo(const bool stereo){
	if(stereo){
		data[2] &= ~(1UL << 13);
	} else {
		data[2] |= (1UL << 13);
	}
	setData(2);
}

//Done
void RDA5807::setBassBoost(const bool boost){
	if(boost){
		data[2] |= (1UL << 12);
	} else {
		data[2] &= ~(1UL << 12);
	}
}

bool RDA5807::stereoReception(){
	return (status[0] >> 2) & 1;
}

//Done
void RDA5807::setSpacing(const float spacing){
	if(spacing == 0.1){
		data[3] |= 3UL;
	} else if (spacing == 0.05){
		data[3] |= (1UL << 1);
		data[3] &= ~1UL;
	} else if (spacing == 0.2){
		data[3] |= 1UL;
		data[3] &= ~(1UL << 1);
	} else {
		data[3] &= ~3UL;
	}
	channelSpacing = spacing;
	setData();
}

void RDA5807::setFrequency(const float frequency, const bool autoTune){
	if(autoTune){
		data[3] |= (1UL << 4);
	} else {
		data[3] &= ~(1UL << 4);
	}
	data[3] &= ~(0x1FF); //Unset bits representing frequency.
	auto tunableFrequency = 1000;
	if((data[3] >> 2) & 3){
		tunableFrequency = frequency * 10 - 650;
	} else if((data[3] >> 2) & 1 || (data[3] >> 2) & 2){
		tunableFrequency = frequency * 10 - 760;
	} else {
		tunableFrequency = frequency * 10 - 870;
	}
	data[3] |= (tunableFrequency << 6);
	setData(3);
	//STC bit is set high when tuning completes. Tune bit is automatically set low when tuning completes.
}

void RDA5807::setBandLimit(const unsigned int limit){
	data[3] &= ~(3UL << 2);		//Unset bits representing US/European Band Limit.
	if(limit >= 0 && limit <= 3){
		data[3] |= (limit << 2);
	}
	setData(3);
}

void RDA5807::setVolume(unsigned int volume){
	data[5] &= ~(15);
	if(volume <= 15){
		data[5] |= volume;
	}
	setData(5);
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

void RDA5807::init(){
	auto freq = 1007;
	auto freqB = freq - 870;
	setRegister(0x02, 0xC000);
	hwlib::wait_ms(1000);
	setRegister(0x02, 0xC00D);
	hwlib::wait_ms(1000);
	setRegister(0x03, (freqB << 6) + 0x10);
}

//Done
void RDA5807::seekChannel(const unsigned int direction, const bool wrapContinue){
	data[2] |= (1UL << 8);		//Set seek mode
	if(direction > 0){
		data[2] |= (1UL << 9);	//Seek up
	} else {
		data[2] &= ~(1UL << 9);	//Seek down
	}
	if(wrapContinue){
		data[2] &= ~(1UL << 7);
	} else {
		data[2] |= (1UL << 7);
	}
	setData(2);
}