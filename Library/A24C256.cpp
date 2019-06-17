#include "hwlib.hpp"
#include "A24C256.hpp"

A24C256::A24C256(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address):
	bus(bus),
	address(address)
{}

void A24C256::writeByte(unsigned int location, uint8_t value){
	if(location >= 0 && location <= 32767){
		data[0] = location >> 8;
		data[1] = location & 0xFF;
		data[2] = value;
		bus.write(address).write(data, 3);
	}
	hwlib::wait_ms(5); //Datasheet states it can take up to 5ms for the chip to become responsive again.
}

uint8_t A24C256::readByte(unsigned int location){
	if(location >= 0 && location <= 32767){
		uint8_t receivedData;
		data[0] = location >> 8;
		data[1] = location & 0xFF;
		bus.write(address).write(data, 2);
		bus.read(address).read(&receivedData, 1);
		return receivedData;
	} else {
		return -1;
	}
}

void A24C256::write(unsigned int location, char* value){
	unsigned int length = 0;
	while(value[length]){
		length++;
	}
	data[0] = location >> 8;
	data[1] = location & 0xFF;
	if(length + (location % 32) <= 31){
		for(unsigned int i = 0; i < length; i++){
			data[i+2] = uint8_t(value[i]);
		}
		bus.write(address).write(data, length+2);
	} else {
		unsigned int cycles = 0;
		if(float((float(length) + (float(location % 32)) / 32) - length) > 0.0){
			cycles = ((length + (location % 32)) / 32)  + 1;
		} else {
			cycles = (length + (location % 32)) / 32;
		}
		for(unsigned int i = 0; i < cycles; i++){
			if(cycles != i){
				for(unsigned int j = 0; j < 31; j++){
					data[j+2] = uint8_t(value[j*i]);
				}
				bus.write(address).write(data, 32);
			} else {
				for(unsigned int j = 0; j < length % 32; j++){
					data[j+2] = uint8_t(value[j*i]);
				}
				bus.write(address).write(data, (length % 32 + 2));
			}
		}
	}
	hwlib::wait_ms(5);
}

