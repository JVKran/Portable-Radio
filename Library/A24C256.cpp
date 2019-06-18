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

void A24C256::write(unsigned int location, char* value, bool largeBuffer){
	unsigned int pageSize;
	if (largeBuffer) {
		pageSize = 64;
	} else {
		pageSize = 32;
	}
	unsigned int length = 0;
	while(value[length]){
		length++;
	}
	if(length + (location % pageSize) <= pageSize-1){
		data[0] = location >> 8;
		data[1] = location & 0xFF;
		for(unsigned int i = 0; i < length; i++){
			data[i+2] = uint8_t(value[i]);
		}
		bus.write(address).write(data, length+2);
		hwlib::wait_ms(5);
	} else {
		unsigned int cycles = 0;
		if(float((float(length) + (float(location % pageSize)) / pageSize) - length) > 0.0){
			cycles = ((length + (location % pageSize)) / pageSize)  + 1;
		} else {
			cycles = (length + (location % pageSize)) / pageSize;
		}
		for(unsigned int i = 0; i <= cycles; i++){
			data[0] = location >> 8;
			data[1] = location & 0xFF;
			if(cycles > i){
				for(unsigned int j = 0; j <= pageSize; j++){
					data[j+2] = uint8_t(value[j+(i * pageSize)]);
				}
				location+=pageSize;
				bus.write(address).write(data, pageSize + 2);
				hwlib::wait_ms(5);
			} else {
				for(unsigned int j = 0; j < length % pageSize; j++){
					data[j+2] = uint8_t(value[j+(cycles * pageSize)]);
				}
				location += length % pageSize;
				bus.write(address).write(data, (length % pageSize + 2));
				hwlib::wait_ms(5);
			}
		}
	}
	hwlib::wait_ms(5);
}

uint8_t A24C256::read(unsigned int location, unsigned int length, uint8_t receivedData[]){
	data[0] = location >> 8;
	data[1] = location & 0xFF;
	bus.write(address).write(data, 2);
	bus.read(address).read(receivedData, length);
	return *receivedData;
}
