#ifndef __RDA5807_HPP
#define __RDA5807_HPP

#include "Radio.hpp"

class RDA5807 : public Radio{
	private:

	public:
		RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x10, int bandLimit = 0);
}

#endif //__RDA5807_HPP