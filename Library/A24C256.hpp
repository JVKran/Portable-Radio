#ifndef __A24C256_HPP
#define __A24C256_HPP

class A24C256{
	private:
		hwlib::i2c_bus & bus;
		const uint8_t address;
		uint8_t data[5] = {};
	public:
		A24C256(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x50);
		void writeByte(uint8_t location, uint8_t value);
		uint8_t readByte(uint8_t location);
};

#endif //__A24C256_HPP