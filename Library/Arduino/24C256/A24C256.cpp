#include <Arduino.h>
#include <Wire.h>
#include "A24C256.hpp"

A24C256::A24C256(uint8_t address):
	address(address)
{}

void A24C256::writeByte(unsigned int location, uint8_t value){
	if(location >= 0 && location <= 32767){
        Wire.beginTransmission(address);
        Wire.write((int)(location >> 8));
        Wire.write((int) (location & 0xFF));
        Wire.write(value);
        Wire.endTransmission();
	}
	delay(5); //Datasheet states it can take up to 5ms for the chip to become responsive again.
}

uint8_t A24C256::readByte(unsigned int location){
	if(location >= 0 && location <= 32767){
		uint8_t receivedData;
		data[0] = location >> 8;
		data[1] = location & 0xFF;
        Wire.beginTransmission(address);
        Wire.write((int)(location >> 8));
        Wire.write((int) (location & 0xFF));
        Wire.endTransmission();
		Wire.requestFrom(int(address),1);
        if (Wire.available()) receivedData = Wire.read();
		return receivedData;
	} else {
		return -1;
	}
}
/*
void A24C256::write(unsigned int location, char* value){
	unsigned int length = 0;
	while(value[length]){
		length++;
	}
	if(length + (location % 32) <= 29){
		data[0] = location >> 8;
		data[1] = location & 0xFF;
		for(unsigned int i = 0; i < length; i++){
			data[i+2] = uint8_t(value[i]);
		}
		bus.write(address).write(data, length+2);
		hwlib::wait_ms(5);
	} else {
		unsigned int cycles = 0;
		unsigned int start = 0;
		if(float((float(length) + (float(location % 32)) / 32) - length) > 0.0){
			cycles = ((length + (location % 32)) / 32)  + 1;
		} else {
			cycles = (length + (location % 32)) / 32;
		}
		for(unsigned int i = 0; i <= cycles; i++){
			data[0] = location >> 8;
			data[1] = location & 0xFF;
			if(cycles > i){
				if(i == 0){
					start = 0;
				} else {
					start = 1;
				}
				for(unsigned int j = 0; j <= 32; j++){
					data[j+2] = uint8_t(value[j+(i * 32)]);
				}
				location+=32;
				bus.write(address).write(data, 32);
				hwlib::wait_ms(5);
			} else {
				for(unsigned int j = 1; j <= length % 32; j++){
					data[j+2] = uint8_t(value[j+(cycles * 32)]);
				}
				location += length % 33;
				bus.write(address).write(data, (length % 32 + 2));
				hwlib::wait_ms(5);
			}
		}
	}
	hwlib::wait_ms(5);
}

 */