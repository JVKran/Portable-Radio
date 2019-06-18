#ifndef __A24C256_HPP
#define __A24C256_HPP

class A24C256{
	private:
		hwlib::i2c_bus & bus;
		const uint8_t address;
		uint8_t data[65] = {};
	public:
		A24C256(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x50);
		void writeByte(unsigned int location, uint8_t value);
		void write(unsigned int location, char* value, bool largeBuffer = true); 		//Should be 32 or 64
		uint8_t readByte(unsigned int location);
		//char* read(unsigned int location, unsigned int length);
};

#endif //__A24C256_HPP