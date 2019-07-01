/// @file

#include "hwlib.hpp"
#include "Radio.hpp"

/// \brief
/// Constuctor
/// \details
/// This constructor has only one parameter; the I2C bus. The address is optional because both
/// the TEA5767 and RDA58XX Series (through a compatibility mode) support communication via 0x60.
/// Though the address can also be set manually or to 0x10 for the RDA5807's full feature package.
/// One can also pass the bandlimit:
///		- TEA5767
///			-# 0 (US/EU)
///			-# 1 (Japan)
///		- RDA5807
///			-# 0 (US/EU)
///			-# 1 (Japan)
///			-# 2 (World Wide)
///			-# 3 (East Europe)
Radio::Radio(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address, int bandLimit):
	bus(bus),
	address(address),
	bandLimit(bandLimit)
{}
