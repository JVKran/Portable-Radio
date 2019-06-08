#include "hwlib.hpp"
#include "TEA5767.hpp"

TEA5767::TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address, bool bandLimit):
	bus(bus),
	address(address),
	bandLimit(bandLimit)
{}

void TEA5767::setData(){
	bus.write(address).write(data, 5);
}

void TEA5767::getStatus(){
	bus.read(address).read(status, 5);
}

void TEA5767::setBandLimit(bool limit){
	bandLimit = limit;
	if(limit){
		data[3] |= (1UL << 5);
	} else {
		data[3] &= ~(1UL << 5);
	}
}

void TEA5767::setHiLo(float frequency, int hilo){
	int pllFrequency;
	if(hilo == 1){
		pllFrequency = (float(4) * (frequency + 0.255) / 0.032768);
		data[2] |= (1UL << 4);
	} else {
		pllFrequency = (float(4) * (frequency - 0.255) / 0.032768);
		data[2] &= ~(1UL << 4);
	}
	data[0] = pllFrequency >> 8;
	data[1] = pllFrequency & 0xFF;
	setMute(false);
	setData();
}

int TEA5767::testHiLo(float frequency){
	setHiLo(frequency, 1);
	hwlib::wait_ms(30);
	int highStrentgh = signalStrength();
	hwlib::cout << "Hi: " << highStrentgh << hwlib::endl;
	setHiLo(frequency, 0);
	hwlib::wait_ms(30);
	int lowStrentgh = signalStrength();
	hwlib::cout << "Lo: " << lowStrentgh << hwlib::endl;
	if (highStrentgh >= lowStrentgh){
		return 1;
	} else {
		return 0;
	}
}


void TEA5767::setFrequency(float frequency, int hiLoForce){
	if((bandLimit && frequency <= 91) || !bandLimit){
		setMute(true);
		if(hiLoForce == -1){
			setHiLo(frequency, testHiLo(frequency));
		} else if(hiLoForce == 0 || hiLoForce == 1){
			setHiLo(frequency, hiLoForce);
		}
	}
}

float TEA5767::getFrequency(){
	getStatus();
	int pllFrequency;
	int frequency;
	if((data[2] >> 4) & 1){//If High side injection is set
		pllFrequency = (data[0] << 8) + data[1];
		frequency = (((pllFrequency / 4.0) * 32768.0) - 225000.0) / 1000000.0;
	} else {
		pllFrequency = (data[0] << 8) + data[1];
		frequency = (((pllFrequency / 4.0) * 32768.0) - 225000.0) / 1000000.0;
	}
	return frequency;
}

void TEA5767::setMute(bool mute){
	if(mute){
		data[0] |= (1UL << 7);
		data[2] |= (3 << 1);
	} else {
		data[0] &= ~(1UL << 7);
		data[2] &= ~(3 << 1);
	}
}

void TEA5767::standBy(bool sleep){
	if(sleep){
		data[3] |= (1UL << 6);
	} else {
		data[3] &= ~(1UL << 6);
	}
}

int TEA5767::signalStrength(){
	setData();
	getStatus();
	return status[3];
}


