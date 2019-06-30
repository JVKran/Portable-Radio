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