#include "hwlib.hpp"
#include "Radio.hpp"

Radio::Radio(hwlib::i2c_bus_bit_banged_scl_sda & bus, int bandLimit, uint8_t address):
	bus(bus),
	bandLimit(bandLimit),
	address(address)
{}