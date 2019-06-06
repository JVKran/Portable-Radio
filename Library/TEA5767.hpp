#ifndef __TEA5767_HPP
#define __TEA5767_HPP

class TEA5767{
	private:
		hwlib::i2c_bus & bus;
		uint8_t address;
		uint8_t data[5] = {0x30, 0x1F, 0x10, 0x10, 0xC0};
		uint8_t status[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
		void setData();
		void getStatus();
		int testHiLo(float frequency);
	public:
		TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x60);
		void setFrequency(float frequency);
		int signalStrength();
};

#endif //__TEA5767_HPP
