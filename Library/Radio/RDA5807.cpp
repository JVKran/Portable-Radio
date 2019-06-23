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
	transaction.write(((value & 0xFF00) >> 8));
	transaction.write((value & 0x00FF));
	hwlib::wait_ms(30);
}

void RDA5807::updateRegister(const int regNumber, const uint16_t value){
	data[regNumber] |= value;
	auto transaction = bus.write(indexAddress);
	transaction.write(regNumber);
	transaction.write(static_cast<uint8_t>((value & 0xFF00) >> 8));
	transaction.write(static_cast<uint8_t>(value & 0x00FF));
	hwlib::wait_ms(30);
}

void RDA5807::getStatus(const uint8_t regNumber){		//Addressing starts at 0x0A
	bus.write(indexAddress).write(regNumber + 0x0A);
	auto transaction = bus.read(indexAddress);
	status[regNumber] = transaction.read_byte() << 8;
	status[regNumber] |= transaction.read_byte();
	hwlib::wait_ms(30);
}

void RDA5807::getStatus(){
	getStatus(0);
	getStatus(1);
	getStatus(2);
	getStatus(3);
	getStatus(4);
	getStatus(5);
	/*	Somehow doesn't work...
	auto transaction = bus.read(address);
	for(unsigned int i = 0; i < 6; i++){
		status[i] = transaction.read_byte() << 8;
		status[i] += transaction.read_byte();
	}
	*/
	hwlib::wait_ms(30);
}

void RDA5807::begin(){
	setMute(false);
	normalAudio(true);
	hwlib::wait_ms(50);
	enableRDS(true);
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
	return (status[1] >> 10);
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
	setData(2);
}

bool RDA5807::stereoReception(){
	getStatus(0);
	return (status[0] >> 10) & 1;
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
	setData(3);
}

bool RDA5807::setFrequency(const float frequency, const bool autoTune){
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
	data[3] |= (tunableFrequency << 6) + 0x10;
	setData(3);
	/*
	hwlib::wait_ms(100);
	getStatus(0);
	return (status[0] >> 14) & 1;
	*/
	return true;
	//STC bit is set high when tuning completes. Tune bit is automatically set low when tuning completes.
}

float RDA5807::getFrequency(){
	int realFrequency;
	int receivedFrequency = (status[0] & 0x3FF); //Only keep last 10 bits
	if((data[3] >> 2) & 3){
		realFrequency = 65.0f;
	} else if((data[3] >> 2) & 1 || (data[3] >> 2) & 2){
		realFrequency = 76.0f;
	} else {
		realFrequency = 87.0f;
	}
	if(data[3] & 3){
		return realFrequency + (receivedFrequency * 0.025);
	} else if(data[3] & 2){
		return realFrequency + (receivedFrequency * 0.05);
	} else if (data[3] & 1){
		return realFrequency + (receivedFrequency * 0.2);
	} else {
		return realFrequency + (receivedFrequency * 0.1);
	}
}

bool RDA5807::isStation(){
	getStatus(1);
	return (status[1] >> 8) & 1;
}

bool RDA5807::isReady(){
	getStatus(1);
	return (status[1] >> 7) & 1;
}

bool RDA5807::isTuned(){
	getStatus(0);
	return (status[0] >> 14) & 1;
}

void RDA5807::setBandLimit(const unsigned int limit){
	data[3] &= ~(3UL << 2);		//Unset bits representing US/European Band Limit.
	if(limit >= 0 && limit <= 3){
		data[3] |= (limit << 2);
	}
	setData(3);
}

void RDA5807::setVolume(const uint8_t volume){
	updateRegister(5, (data[5] & 0xFFF0) | (volume & 0x0F));
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

void RDA5807::getRDS(){
	getStatus();
	hwlib::cout << status[2] << status[3] << status[4] << status[5] << hwlib::endl;
}

void RDA5807::enableRDS(const bool enable){
	if(enable){
		data[2] |= (1UL << 3);
	} else {
		data[2] &= ~(1UL << 3);
	}
	setData(2);
}

bool RDA5807::rdsReady(){
	getStatus(0);
	return (status[0] >> 15) & 1;
}

bool RDA5807::rdsSync(){
	getStatus(0);
	return (status[0] >> 12) & 1;
}

void RDA5807::processRDS(){
	/*
	getStatus(2);
	getStatus(3);
	getStatus(4);
	getStatus(5);
	*/
	getStatus(); 
	hwlib::cout << char(status[2] >> 8)  << char(status[2] & 0xFF) << ", " << char(status[3] >> 8) << char(status[3] & 0xFF) << ", " << char(status[4] >> 8)  << char(status[4] & 0xFF) << ", " << char(status[5] >> 8) << char(status[5] & 0xFF) <<  hwlib::endl;

}

void RDA5807::test(){
	getStatus(0);
	getStatus(1);
	getStatus(2);
	getStatus(3);
	getStatus(4);
	getStatus(5);
	hwlib::cout << status[0] << ", " << status[1] << ", " << status[2] << ", " << status[3] << hwlib::endl;
}