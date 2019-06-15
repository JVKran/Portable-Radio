#include "hwlib.hpp"
#include "TEA5767.hpp"

TEA5767::TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address, bool bandLimit):
	bus(bus),
	address(address),
	bandLimit(bandLimit)
{}

void TEA5767::setData(){
	bus.write(address).write(data, 5);
	hwlib::wait_ms(30);
}

void TEA5767::getStatus(){
	bus.read(address).read(status, 5);
	hwlib::wait_ms(30);
}

void TEA5767::setBandLimit(bool limit){
	bandLimit = limit;
	if(limit){
		data[3] |= (1UL << 5);
	} else {
		data[3] &= ~(1UL << 5);
	}
	setData();
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
	if((((bandLimit && frequency <= 91) || (bandLimit && frequency >= 76) || ((!bandLimit && frequency <= 108) || (!bandLimit && frequency >= 87.5))) && frequency != -1)){
		setMute(true);
		if(hiLoForce == -1){
			setHiLo(frequency, testHiLo(frequency));
		} else if(hiLoForce == 0 || hiLoForce == 1){
			setHiLo(frequency, hiLoForce);
		}
		setMute(false);
	} else {
		if(bandLimit){
			setFrequency(76);
		} else {
			setFrequency(87.5);
		}
	}
}

float TEA5767::getFrequency(){
	getStatus();
	int pllFrequency = ((status[0]&0x3F) << 8) + status[1];
	int frequency;
	if(!(data[2] >> 4) & 1){//If High side injection is set
		frequency = (((pllFrequency / 4.0) * 32768.0) - 225000.0) / 1000000.0;
	} else {
		frequency = (((pllFrequency / 4.0) * 32768.0) + 225000.0) / 1000000.0;
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
	setData();
}

void TEA5767::standBy(bool sleep){
	if(sleep){
		data[3] |= (1UL << 6);
	} else {
		data[3] &= ~(1UL << 6);
	}
	setData();
}

int TEA5767::signalStrength(){
	setData(); //Update values 
	getStatus();
	return status[3];
}

float TEA5767::search(int direction, int qualityThreshold){
	getStatus();
	if(direction == 1){
		setFrequency(getFrequency() + 0.1);
		data[2] |= (1UL << 7);
	} else if (direction == 0){
		setFrequency(getFrequency() - 0.1);
		data[2] &= ~(1UL << 7);
	}
	//Search Mode On
	data[0] |= (1UL << 6);
	//SSL Highest Quality
	data[2] |= (qualityThreshold << 5);
	//Mute Volume
	data[0] |= (1UL << 7);
	data[2] |= (3 << 1);
	//Unset search indicator
	data[3] &= ~(1);
	setData();
	float foundFrequency;
	hwlib::wait_ms(100);
	for(;;){
		getStatus();
		if((status[0] >> 7) & 1){
			foundFrequency = getFrequency();
			break;
		}
	}
	data[0] &= ~(1UL << 6);
	setMute(false);
	return foundFrequency;
}
