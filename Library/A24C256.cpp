#include "hwlib.hpp"
#include "A24C256.hpp"

A24C256::A24C256(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address):
	bus(bus),
	address(address)
{}

void A24C256::writeByte(uint8_t location, uint8_t value){
	if(location >= 0 && location <= 32767){
		data[0] = location >> 8;
		data[1] = location & 0xFF;
		data[2] = value;
		bus.write(address).write(data, 3);
	}
	hwlib::wait_ms(5); //Datasheet states it can take up to 5ms for the chip to become responsive again.
}

uint8_t A24C256::readByte(uint8_t location){
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
