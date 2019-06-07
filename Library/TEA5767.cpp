#include "hwlib.hpp"
#include "TEA5767.hpp"

TEA5767::TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address, int hiLoPrefrence):
	bus(bus),
	address(address),
	hiLoPrefrence(hiLoPrefrence)
{}

void TEA5767::setData(){
	bus.write(address).write(data, 5);
}

void TEA5767::getStatus(){
	bus.read(address).read(status, 5);
}

void TEA5767::setHiLo(float frequency, int hilo){
	int pllFrequency;
	if(hilo == 1){
		pllFrequency = (float(4) * (frequency + 0.255) / 0.032768);
		data[2] |= 1UL << 4;
	} else {
		pllFrequency = (float(4) * (frequency - 0.255) / 0.032768);
		data[2] &= ~(1UL << 4);
	}
	data[0] = pllFrequency >> 8;
	data[1] = pllFrequency & 0xFF;
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

void TEA5767::setPrefrence(int hilo){
	hiLoPrefrence = hilo;
}


void TEA5767::setFrequency(float frequency){
	if(hiLoPrefrence == -1){
		setHiLo(frequency, testHiLo(frequency));
	} else {
		setHiLo(frequency, hiLoPrefrence);
	}
}

int TEA5767::signalStrength(){
	getStatus();
	return status[3];
}


