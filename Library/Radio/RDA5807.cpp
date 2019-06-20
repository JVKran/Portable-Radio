#include "hwlib.hpp"
#include "RDA5807.hpp"

RDA5807::RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address, int bandLimit, unsigned int channelSpacing):
	Radio(bus, address, bandLimit),
	channelSpacing(channelSpacing)
{
	setSpacing(channelSpacing);
}

void RDA5807::setData(){
	bus.write(address).write(data, 12);
	hwlib::wait_ms(30);
}

void RDA5807::getStatus(){
	bus.read(address).read(status, 10);
	hwlib::wait_ms(30);
}

unsigned int RDA5807::signalStrength(){
	getStatus();
	return (status[0] >> 1);
}

void RDA5807::setMute(const bool mute){
	if(mute){
		data[2] &= ~(1UL << 7);
	} else {
		data[2] |= (1UL << 7);
	}
	setData();
}

void RDA5807::setBandLimit(const unsigned int limit){
	if(limit < 4){
		data[5] |= (limit << 2);
	} else {
		data[5] |= (3UL << 2);
	}
	setData();
}

unsigned int RDA5807::hasBandLimit(){
	return int((data[3] >> 2) & 1) + int((data[3] >> 3));
}

void RDA5807::setStereo(const bool stereo){
	if(stereo){
		data[2] &= ~(1UL << 5);
	} else {
		data[2] |= (1UL << 5);
	}
	setData();
}

bool RDA5807::stereoReception(){
	return (status[0] >> 2) & 1;
}

void RDA5807::setSpacing(const unsigned int spacing){
	if(spacing == 25000){
		data[5] |= 3UL;
	} else if (spacing == 50000){
		data[5] |= (1UL << 1);
		data[5] &= ~1UL;
	} else if (spacing == 200000){
		data[5] |= 1UL;
		data[5] &= ~(1UL << 1);
	} else {
		data[5] &= ~3UL;
	}
	setData();
}

void RDA5807::setFrequency(const float frequency){
	unsigned int bandLimit = hasBandLimit();
	uint8_t settableFrequency;
	if(bandLimit == 0){
		settableFrequency = channelSpacing * frequency + 87000000;
	} else if (bandLimit == 3){
		settableFrequency = channelSpacing * frequency + 76000000;
	} else {
		settableFrequency = channelSpacing * frequency + 65000000;
	}
	data[4] = settableFrequency;
	setData();
}

void RDA5807::setVolume(unsigned int volume){
	data[5] &= ~(15);
	if(volume <= 15){
		data[5] |= volume;
	}
	setData();
}

void RDA5807::tune(){
	data[2] |= (1UL << 4);
	setData();
}

void RDA5807::standBy(const bool standby){
	if(standby){
		data[1] &= ~(1UL);
	} else {
		data[1] |= 1UL;
	}
}

void RDA5807::normalOutput(const bool normal){
	if(normal){
		data[1] |= (1UL << 15);
	} else {
		data[1] &= ~(1UL << 15);
	}
}