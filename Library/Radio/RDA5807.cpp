#include "hwlib.hpp"
#include "RDA5807.hpp"

RDA5807::RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address, int bandLimit, float channelSpacing):
	Radio(bus, address, bandLimit),
	indexAddress(address + 1),		//0x10 for sequential access and 0x11 for indexed access
	channelSpacing(channelSpacing),
	radioData(radioDataSystem(bus))
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
	transaction.write((data[regNumber] & 0xFF00) >> 8);
	transaction.write(data[regNumber] & 0x00FF);
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
	bus.write(indexAddress).write(0x0A + regNumber);
	auto transaction = bus.read(indexAddress);
	status[regNumber] = transaction.read_byte() << 8;
	status[regNumber] |= transaction.read_byte();
	hwlib::wait_ms(30);
}

void RDA5807::getStatus(){
	bus.write(indexAddress).write(0x0A);
	auto transaction = bus.read(indexAddress);
	for(unsigned int i = 0; i < 6; i++){
		status[i] = transaction.read_byte() << 8;
		status[i] |= transaction.read_byte();
	}
	hwlib::wait_ms(30);
}

void RDA5807::begin(){
	hwlib::wait_ms(1000);
	setMute(false);
	normalAudio(true);
	setTune(true);
	hwlib::wait_ms(50);
	enableRadioData(true);
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
	status[1] &= ~0xFC00;
	getStatus(1);
	return ((status[1] & 0xFC00) >> 9);
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

bool RDA5807::isMuted(){
	return !(data[2] >> 14) & 1;
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
void RDA5807::setBassBoost(const bool boost){
	if(boost){
		data[2] |= (1UL << 12);
	} else {
		data[2] &= ~(1UL << 12);
	}
	setData(2);
}

bool RDA5807::bassBoosted(){
	return (data[2] >> 12) & 1;
}

bool RDA5807::stereoReception(){
	getStatus(0);
	return (status[0] >> 10) & 1;
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

bool RDA5807::isStereo(){
	return !(data[2] >> 13) & 1;		//Stereo is 0, mono is 1
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
	radioData.reset();
	if(autoTune){
		data[3] |= (1UL << 4);
	} else {
		data[3] &= ~(1UL << 4);
	}
	data[3] &= ~(0x1FF << 6); //Unset bits representing frequency.
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
	hwlib::wait_ms(100);
	getStatus(0);
	return (status[0] >> 14) & 1;
	//STC bit is set high when tuning completes. Tune bit is automatically set low when tuning completes.
}

float RDA5807::getFrequency(){
	getStatus(0);
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

int RDA5807::getIntFrequency(){
	return (getFrequency() * 10);
}

bool RDA5807::isStation(){
	getStatus(1);
	return (status[1] & 0x4100);
}

bool RDA5807::isReady(){
	getStatus(1);
	return (status[1] & 0x4080);
}

bool RDA5807::isTuned(){
	getStatus(0);
	return (status[0] & 0x4000);
}

void RDA5807::setBandLimit(const unsigned int limit){
	data[3] &= ~(3UL << 2);		//Unset bits representing US/European Band Limit.
	if(limit >= 0 && limit <= 3){
		data[3] |= (limit << 2);
	}
	setData(3);
}

void RDA5807::setVolume(const uint8_t volume){
	data[5] &= ~0x000F;
	data[5] |= (volume & 0x000F);
	setData(5);
}

void RDA5807::setTune(const bool tune){
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

bool RDA5807::isStandBy(){
	return !(data[2] & 1);
}

//Done
void RDA5807::seekChannel(const unsigned int direction, const bool wrapContinue){
	radioData.reset();
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

void RDA5807::setSeekThreshold(const uint8_t threshold){
	data[5] |= (threshold & 0xF00);
	setData(5);
}

bool RDA5807::seekCompleted(){
	getStatus(0);
	return (status[0] >> 14) & 1;
}

void RDA5807::enableRadioData(const bool enable){
	if(enable){
		data[2] |= (1UL << 3);
	} else {
		data[2] &= ~(1UL << 3);
	}
	setData(2);
}

bool RDA5807::radioDataReady(){
	getStatus(0);
	return (status[0] >> 15) & 1;
}

bool RDA5807::radioDataSynced(){
	getStatus(0);
	return (status[0] >> 12) & 1;
}
int RDA5807::radioDataErrors(const int block){
	getStatus(block);
	if(block == 1){
		return (status[1] & 0x000C);
	} else {
		return (status[1] & 0x0003);
	}
}

void RDA5807::printRawRadioData(){
	radioData.rawData();
}

char* RDA5807::stationName(){
	return radioData.stationName();
}

char* RDA5807::getStationText(){
	return radioData.getStationText();
}

char* RDA5807::stationText(){
	return radioData.stationText();
}

void RDA5807::updateRadioData(){
	radioData.update();
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