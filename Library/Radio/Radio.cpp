#include "hwlib.hpp"
#include "Radio.hpp"

Radio::Radio(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address, int bandLimit):
	bus(bus),
	address(address),
	bandLimit(bandLimit)
{}