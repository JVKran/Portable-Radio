#include "hwlib.hpp"
#include "A24C256.hpp"

A24C256::A24C256(hwlib::i2c_bus_bit_banged_scl_sda & bus, unsigned int memorySize, uint8_t address):
	bus(bus),
	memorySize(memorySize*128),							//Verify how many addresses are available
	address(address)
{}

void A24C256::write(unsigned int location, uint8_t value){
	if(location >= 0 && location < memorySize){
		data[0] = location >> 8;						//MSB Location
		data[1] = location & 0xFF;						//LSB Location
		data[2] = value;								//Value to save
		bus.write(address).write(data, 3);
	}
	hwlib::wait_ms(5); 									//Datasheet states it can take up to 5ms for the chip to become responsive again.
}

uint8_t A24C256::read(unsigned int location){
	if(location >= 0 && location < memorySize){
		uint8_t receivedData;
		data[0] = location >> 8;
		data[1] = location & 0xFF;
		bus.write(address).write(data, 2);				//Tell which address to get data from
		bus.read(address).read(&receivedData, 1);		//Retrieve data
		return receivedData;
	} else {
		return 0;
	}
}

void A24C256::write(unsigned int location, char* value, bool largeBuffer){
	if(location >= 0 && location < memorySize){
		unsigned int pageSize;
		/*						Buffer-Size
		A Buffer-size of 64 bytes is faster and more efficient.
		Though not all microcontrollers or I2C libraries can handle this.
		To make this library user-friendly you can choose which one to use.
		*/
		if (largeBuffer) {
			pageSize = 64;
		} else {
			pageSize = 32;
		}
		unsigned int length = 0;
		while(value[length]){
			length++;
		}
		/*						Pages
		The memory consists of pages of 64 bytes each. When an
		edge of a page is reached, the write process will continue
		at the beginning of this current page; thus overwriting data.
		To prevent that from happening, we monitor when this will happen.
		We'll write multiple bytes of amount 'pageSize' or as much that
		will fit in the current page. If there is data left, we'll write
		that to the next page.
		*/
		if(length + location < memorySize){						//Check whether all data will actually fit whithout overwriting data at address 0,1,...
			if(length + (location % pageSize) < pageSize){		//If the date doesn't cross a page-boundary
				data[0] = location >> 8;
				data[1] = location & 0xFF;
				for(unsigned int i = 0; i < length; i++){
					data[i+2] = uint8_t(value[i]);
				}
				bus.write(address).write(data, length+2);
				hwlib::wait_ms(5);
			} else {
				unsigned int cycles = 0;
				if(float((float(length) + (float(location % pageSize)) / pageSize) - length) > 0.0){		//If there is data left to save;
					cycles = ((length + (location % pageSize)) / pageSize)  + 1;							//add another write-cycle
				} else {
					cycles = (length + (location % pageSize)) / pageSize;
				}
				for(unsigned int i = 0; i <= cycles; i++){
					data[0] = location >> 8;												//Update location from where we can start again; past the page-boundary
					data[1] = location & 0xFF;
					if(cycles > i){
						for(unsigned int j = 0; j <= pageSize; j++){						//Put the max amount of data in buffer until page-boundary is reached
							data[j+2] = uint8_t(value[j+(i * pageSize)]);					// Reminder: first two bytes form the beginning location to save the data
						}
						location+=pageSize;
						bus.write(address).write(data, pageSize + 2);
						hwlib::wait_ms(5);
					} else {																//If last cycle is reached we only need to write the left-over bytes;
						for(unsigned int j = 0; j < length % pageSize; j++){				//amount % 64 = amount to write in last cycle
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
	}
}

uint8_t A24C256::read(unsigned int location, unsigned int length, uint8_t receivedData[]){
	if(location >= 0 && location < memorySize){
		data[0] = location >> 8;
		data[1] = location & 0xFF;
		bus.write(address).write(data, 2);
		bus.read(address).read(receivedData, length);				//Read multiple bytes at once
		return *receivedData;
	} else {
		return 0;
	}
}
