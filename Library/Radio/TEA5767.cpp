#include "hwlib.hpp"
#include "TEA5767.hpp"

TEA5767::TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, int bandLimit, uint8_t address):
	Radio(bus, bandLimit, address)
{}

void TEA5767::setData(){
	bus.write(address).write(data, 5);
	hwlib::wait_ms(30);
}

void TEA5767::getStatus(){
	bus.read(address).read(status, 5);
	hwlib::wait_ms(30);
}

void TEA5767::setBandLimit(int limit){
	bandLimit = limit;
	if(limit > 0){
		data[3] |= (1UL << 5);
	} else {
		data[3] &= ~(1UL << 5);
	}
	setData();
}

void TEA5767::setHiLo(float frequency, int hilo){
	unsigned int pllFrequency;
	if(hilo == 1){
		pllFrequency = (4 * (frequency * 1000000 + 225000) / 32768);
		data[2] |= (1UL << 4);
	} else {
		pllFrequency = (4 * (frequency * 1000000 - 225000) / 32768);
		data[2] &= ~(1UL << 4);
	}
	data[0] = pllFrequency >> 8;
	data[1] = pllFrequency & 0xFF;
}

int TEA5767::testHiLo(float frequency){
	setHiLo(frequency, 1);
	setData();
	hwlib::wait_ms(30);
	int highStrentgh = signalStrength();
	hwlib::cout << "Hi: " << highStrentgh << hwlib::endl;
	setHiLo(frequency, 0);
	setData();
	hwlib::wait_ms(30);
	int lowStrentgh = signalStrength();
	hwlib::cout << "Lo: " << lowStrentgh << hwlib::endl;
	if (highStrentgh >= lowStrentgh){
		hwlib::cout << "Selected High Side Injection" << hwlib::endl;
		return 1;
	} else {
		hwlib::cout << "Selected Low Side Injection" << hwlib::endl;
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
	setData();
}

float TEA5767::getFrequency(){
	getStatus();
	int pllFrequency = ((status[0]&0x3F) << 8) + status[1];
	if((data[2] >> 4) & 1){//If High side injection is set
		return (((pllFrequency / 4.0) * 32768.0) - 225000.0) / 1000000.0;
	} else {
		return (((pllFrequency / 4.0) * 32768.0) + 225000.0) / 1000000.0;
	}
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

void TEA5767::setStereo(bool stereo){
	if (stereo){
		data[2] &= ~(1UL << 3);
	} else {
		data[2] |= (1UL << 3);
	}
}

bool TEA5767::stereoReception(){
	getStatus();
	return (status[2] >> 7) & 1;
}

void TEA5767::audioSettings(bool SNC, bool HCC, bool SM){
	if (SNC){
		data[3] |= (1UL << 1);
	} else {
		data[3] &= ~(1UL << 1);
	}
	if (HCC){
		data[3] |= (1UL << 2);
	} else {
		data[3] &= ~(1UL << 2);
	}
	if (SM){
		data[3] |= (1UL << 3);
	} else {
		data[3] &= ~(1UL << 3);
	}
}

void TEA5767::search(int direction, int qualityThreshold){
	getStatus();
	float startFrequency = getFrequency();
	//set HiLo
	data[2] |= (1UL << 4);
	if(direction == 1){
		setHiLo(getFrequency() + 0.1, 1);
		data[2] |= (1UL << 7);
	} else if (direction == 0){
		setHiLo(getFrequency() - 0.1, 1);
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
	//Set XTAL to 1 and PLLREF to 0 for 32768kHz Clock Frequency
	//data[3] |= (1UL << 4);
	//data[4] &= ~(1UL << 7);
	setData();
	float foundFrequency = startFrequency;
	hwlib::wait_ms(100);
	for(;;){
		getStatus();
		//If Station has been Found, BandLimit has not been reached and another frequency is found; tune to that.
		if((status[0] >> 7) & 1 && (getFrequency() > startFrequency + 0.1 || getFrequency() < startFrequency - 0.1)){
			data[2] |= (1UL << 4);
			foundFrequency = getFrequency();
			break;
		} else if ((status[0] >> 6) & 1){		//If bandlimit has been reached
			if(direction == 1){
				if(bandLimit){
					setHiLo(76, 1);
				} else {
					setHiLo(88, 1);
				}
				setData();
			} else {
				if(bandLimit){
					setHiLo(91, 1);
				} else {
					setHiLo(108, 1);
				}
				setData();
			}
		} else {
			if(direction == 1){
				setHiLo(getFrequency() + 0.1, 1);
			} else if (direction == 0){
				setHiLo(getFrequency() - 0.1, 1);
			}
			setData();
		}
		hwlib::cout << int(getFrequency()) << ", " << int(foundFrequency) << hwlib::endl;
	}
	data[0] &= ~(1UL << 6);
	setMute(false);
	getStatus();
	setFrequency(foundFrequency);
}