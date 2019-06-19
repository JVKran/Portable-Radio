#include "hwlib.hpp"
#include "TEA5767.hpp"
#include <stdio.h>

int main( void ){
    namespace target = hwlib::target; 
    
    auto scl = target::pin_oc( target::pins::d8 );
    auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto radio = TEA5767(i2c_bus);
	radio.setStereo(true);
	radio.audioSettings(false, true, true);

	hwlib::cout << "Tuning to 100.7" << hwlib::endl;
	radio.setFrequency(100.7);
	for(unsigned int i = 0; i < 5; i++){
		hwlib::wait_ms(2000);
		hwlib::cout << radio.signalStrength() << hwlib::endl;
	}

	hwlib::cout << "Auto Search Test Procedure is Starting" << hwlib::endl;
	radio.setFrequency(86);
	hwlib::wait_ms(2000);
	std::array<float, 10> stations = {};
	for(unsigned int i = 0; i < 10; i++){
		radio.search(1, 3);
		stations[i] = int(radio.getFrequency());
		hwlib::cout << hwlib::setw(25) << "Frequency: " << hwlib::setw(8) << int(stations[i]) << hwlib::endl;
		hwlib::cout << hwlib::setw(25) << "Signal Strength: " << hwlib::setw(8) << radio.signalStrength() << hwlib::endl << hwlib::endl;
		hwlib::wait_ms(2000);
	}

	for(const float & element : stations){
		hwlib::cout << int(element) << hwlib::endl;
		radio.setFrequency(element);
		hwlib::wait_ms(2000);
	}

	radio.setFrequency(100.7);

}
