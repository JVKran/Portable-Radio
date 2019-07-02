/// @file

#ifndef __A24C256_HPP
#define __A24C256_HPP

/// \brief
/// 24C EEPROM Interface
/// \details
/// This is an interface that simplifies writing to and reading from 24CXXXX EEPROM chips.
/// Completely compatible with 24C24C32, 24C64, 24C128, 24C256, 24C512 and 24C1024. It is
/// possible to write and read single bytes (chars, bools, integers) and multi-byte values (char[]). By default
/// , the chip is unable to write multi-byte values accross multiple pages. However, this is dealt with
/// from within this library.
/// 
///	All supported operations are:
///		- Write Single Byte
///		- Write Multiple Bytes
///		- Read Single Byte
///		- Read Multiple Bytes
/// 
/// ~~~~~~~~~~~~~~~{.cpp}
/// #include "hwlib.hpp"
/// #include "A24C256.hpp"
/// 
/// int main( void ){
///     namespace target = hwlib::target; 
/// 
///     auto scl = target::pin_oc( target::pins::d8 );
///     auto sda = target::pin_oc( target::pins::d9 );
/// 	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
/// 
/// 	auto memory = A24C256(i2c_bus);
/// 	auto largeMemory = A24C256(i2c_bus, 512);
/// 
/// 	//Write one single byte at location 0 and retrieve it
/// 	memory.write(0, 'c');
/// 	hwlib::cout << char(memory.read(0)) << hwlib::endl;
/// 
/// 	//Write 12 bytes to location 20
/// 	char data[]={"Hello World!"};
/// 	memory.write(20, data);
/// 
/// 	//Retrieve multiple bytes (preffered)
/// 	uint8_t receivedData[12];
/// 	memory.read(20, 12, receivedData);
/// 	for(unsigned int i = 0; i < 12; i++){
/// 		hwlib::cout << char(receivedData[i]);
/// 	}
/// 
/// 	hwlib::cout << hwlib::endl;
/// 
/// 	//Another way to retrieve multiple bytes
/// 	for(unsigned int i = 20; i < 32; i++){
/// 		hwlib::cout << char(memory.read(i));
/// 	}
/// 
/// 	hwlib::cout << hwlib::endl;
/// 
/// 	memory.write(0, 100);
/// 	hwlib::cout << memory.read(0) << hwlib::endl;
/// 
/// 	memory.write(0, true);
/// 	hwlib::cout << (bool(memory.read(0)) && 1);
/// 
/// }
/// ~~~~~~~~~~~~~~~
class A24C256{
	private:
		hwlib::i2c_bus & bus;
		unsigned int memorySize;	//This library is also compatible with 24C24C32, 24C64, 24C65, 24C128, 24C256, 24C512, 24C1024
		const uint8_t address;
		uint8_t data[65] = {};		//Two address bytes followed by data to save
	public:
		A24C256(hwlib::i2c_bus_bit_banged_scl_sda & bus, unsigned int givenMemorySize = 256, uint8_t address = 0x50);

		void write(unsigned int location, uint8_t value);
		void write(unsigned int location, char* value, bool largeBuffer = true);

		uint8_t read(unsigned int location);
		uint8_t read(unsigned int location, unsigned int length, uint8_t receivedData[] = {});

		uint8_t getAddress();
		unsigned int getMemorySize();
};

#endif //__A24C256_HPP
