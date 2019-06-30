/// @file

#include "hwlib.hpp"
#include "A24C256.hpp"

/// \brief
/// Constuctor
/// \details
/// This constructor has one mandatory parameter; the I2C bus. The Address defaults to 0x50.
/// The memorysize defaults to 256Kb. Changing this makes the use of other 24CXXXX chips possible. If 
/// an illegal amount of memory is given, the constructor will change the memory size to a valid
/// size by determining which valid size is closest to the given one.
A24C256::A24C256(hwlib::i2c_bus_bit_banged_scl_sda & bus, unsigned int givenMemorySize, uint8_t address):
	bus(bus),
	memorySize(givenMemorySize),
	address(address)
{
	bool validSize = (memorySize & (memorySize - 1)) == 0;	//If it is a power of 2 and substract 1 all bits before that become 1.
	if(!validSize){											// 10000000 & 0111111 = 0 while 10011010 & 01101101 = 4
		if(memorySize < 45){
			memorySize = 32;
		} else if (memorySize < 90){
			memorySize = 64;
		} else if (memorySize < 190){
			memorySize = 128;
		} else if (memorySize < 400){
			memorySize = 256;
		} else if (memorySize < 800){
			memorySize = 512;
		} else {
			memorySize = 1024;
		}
	}
	memorySize *= 128;									//Verify how many addresses are available
}

/// \brief
/// Single Byte Write
/// \details
/// This function takes two mandatory parameters; location and value.
/// The location has to be valid for the value to be written. The location
/// is divided in two parts; MSB and LSB. The location and value are written 
/// at once. It takes 5ms for the EEPROM to become responsive again. 
/// Waiting these 5ms prevents weird things from happening.
void A24C256::write(unsigned int location, uint8_t value){
	if(location >= 0 && location < memorySize){
		data[0] = location >> 8;						//MSB Location
		data[1] = location & 0xFF;						//LSB Location
		data[2] = value;								//Value to save
		bus.write(address).write(data, 3);
	}
	hwlib::wait_ms(5); 									//Datasheet states it can take up to 5ms for the chip to become responsive again.
}

/// \brief
/// Single Byte read
/// \details
/// This function takes one mandatory parameter; the location. If the location 
/// is valid, it returns the value that is currently saved at the desired position.
/// Otherwise, it returns 0 to emphasize failure.
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

/// \brief
/// Multi Byte write
/// \details
/// This function takes a couple of arguments. The location is the address where the start
/// of the value to be saved is going to be written. The value is going to be saved (beginning
/// at location) and largeBuffer defines if the buffer is 64 or 32 bytes. If the location
/// is valid, the data will be written.
/// 
///						Buffer
/// A Buffer-size of 64 bytes is faster and more efficient.
/// Though not all microcontrollers or I2C libraries can handle this.
/// To make this library user-friendly you can choose which one to use.
/// 
///						Pages
/// The memory consists of pages of 64 bytes each. When an
///	edge of a page is reached, the write process will continue
///	at the beginning of this current page; thus overwriting data.
///	To prevent that from happening, we monitor when this will happen.
///	We'll write multiple bytes of amount 'pageSize' or as much that
///	will fit in the current page. If there is data left, we'll write
///	that to the next page.
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
		if(length + location < memorySize){						//Check whether all data will actually fit whithout overwriting data at the beginning of the page.
			if(length + (location % pageSize) < pageSize){		//If the date doesn't cross a page-boundary;
				data[0] = location >> 8;						//Just write as if it is a normal write operation.
				data[1] = location & 0xFF;
				for(unsigned int i = 0; i < length; i++){
					data[i+2] = uint8_t(value[i]);
				}
				bus.write(address).write(data, length+2);
				hwlib::wait_ms(5);
			} else {																						//If it doesn't fit in one page.
				unsigned int cycles = (length + (location % pageSize)) / pageSize;							//Calculate over how many pages the data has to be distributed.
				if(float((float(length) + (float(location % pageSize)) / pageSize) - length) > 0.0){		//If there is data left to save;
					cycles++;																				//add another write-cycle
				}
				for(unsigned int i = 0; i <= cycles; i++){
					data[0] = location >> 8;
					data[1] = location & 0xFF;
					//If there has to be another write after this one; there is data left.
					if(cycles > i){
						for(unsigned int j = 0; j <= pageSize; j++){						//Put the max amount of data in buffer until page-boundary is reached
							data[j+2] = uint8_t(value[j+(i * pageSize)]);					// Reminder: first two bytes form the beginning location to save the data
						}
						location+=pageSize;
						bus.write(address).write(data, pageSize + 2);
						hwlib::wait_ms(5);
					//Last write operation; only beginning is data to be saved.
					} else {																//If last cycle is reached we only need to write the left-over bytes;
						for(unsigned int j = 0; j < length % pageSize; j++){				//amount % 64 = amount to write in last cycle
							data[j+2] = uint8_t(value[j+(cycles * pageSize)]);
						}
						location += length % pageSize;										//Update location from where we can start again; past the page-boundary
						bus.write(address).write(data, (length % pageSize + 2));
						hwlib::wait_ms(5);
					}
				}
			}
			hwlib::wait_ms(5);
		}
	}
}


/// \brief
/// Multi Byte read
/// \details
/// This function takes a couple of arguments. The location is the address where to start
/// reading from. The length is the amount of bytes to read and receivedData is the variable
/// to store data in. If the location is valid, the data will be returned. Otherwise 0
/// will be returned to emphasize failure.
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

/// \brief
/// Get I2C address
/// \details
/// This function returns the set I2C address of the chip.
uint8_t A24C256::getAddress(){
	return address;
}

/// \brief
/// Get Memory Size
/// \details
/// This function returns the set memory size of the chip.
unsigned int A24C256::getMemorySize(){
	return memorySize / 128;
}
