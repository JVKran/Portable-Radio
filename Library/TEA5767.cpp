#include "hwlib.hpp"
#include "TEA5767.hpp"

TEA5767::TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address):
	bus(bus),
	address(address)
{}

void TEA5767::setData(){
	bus.write(address).write(data, 5);
}

void TEA5767::getStatus(){
	bus.read(address).read(status, 5);
}

int TEA5767::testHiLo(float frequency){
	data[0] |= 1 << 7; 				//Mute
	int pllFrequency = (float(4) * (frequency + 0.255) / 0.032768);
	data[0] = pllFrequency >> 8;
	data[1] = pllFrequency & 0xFF;
	data[2] |= 1 << 4;				//High Side Injection
	setData();
	hwlib::wait_ms(30);
	int highStrentgh = signalStrength();
	pllFrequency = (float(4) * (frequency - 0.255) / 0.032768);
	data[0] = pllFrequency >> 8;
	data[1] = pllFrequency & 0xFF;
	data[2] &= ~(1 << 4);			//Low side injection
	setData();
	hwlib::wait_ms(30);
	int lowStrentgh = signalStrength();
	if (highStrentgh < lowStrentgh){
		return 1;
	} else {
		return 0;
	}
}


void TEA5767::setFrequency(float frequency){
	data[2] &= ~(1 << 7);			//Unmute
	if(testHiLo(frequency) == 1){	//Already ends low so only needs to be set high if that's better
		int pllFrequency = (float(4) * (frequency + 0.255) / 0.032768);
		data[0] = pllFrequency >> 8;
		data[1] = pllFrequency & 0xFF;
		data[2] |= 1 << 4;
	}
	setData();
}

int TEA5767::signalStrength(){
	getStatus();
	return status[3];
}


