#ifndef __TEA5767_HPP
#define __TEA5767_HPP

class TEA5767{
	private:
		hwlib::i2c_bus & bus;
		uint8_t address;
	public:
		TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x60);
		void test();
};

#endif //__TEA5767_HPP
