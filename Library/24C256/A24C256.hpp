#ifndef __A24C256_HPP
#define __A24C256_HPP

class A24C256{
	private:
		hwlib::i2c_bus & bus;
		unsigned int memorySize;	//This library is also compatible with 24C24C32, 24C64, 24C65, 24C128, 24C256, 24C512, 24C1024
		const uint8_t address;
		uint8_t data[65] = {};		//Two address bytes followed by data to save
	public:
		A24C256(hwlib::i2c_bus_bit_banged_scl_sda & bus, unsigned int memorySize = 256, uint8_t address = 0x50);
		void write(unsigned int location, uint8_t value);
		void write(unsigned int location, char* value, bool largeBuffer = true); 		//Should be 32 or 64
		uint8_t read(unsigned int location);
		uint8_t read(unsigned int location, unsigned int length, uint8_t receivedData[] = {});
};

#endif //__A24C256_HPP