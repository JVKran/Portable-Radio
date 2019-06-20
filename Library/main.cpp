#include "hwlib.hpp"
#include "TEA5767.hpp"
#include <stdio.h>

int main( void ){
    namespace target = hwlib::target; 
    
    auto scl = target::pin_oc( target::pins::d8 );
    auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto radio = TEA5767(i2c_bus);

	hwlib::cout << "Tuning to 100.7FM (Q-Music): ";
	radio.setFrequency(100.7);
	hwlib::cout << "DONE" << hwlib::endl << hwlib::boolalpha << "Reception Quality:";

	for(unsigned int i = 0; i < 10; i++){
		hwlib::cout << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << hwlib::endl;
		hwlib::wait_ms(1000);
	}

	hwlib::cout << "Setting Mono Signal: ";
	radio.setStereo(false);
	hwlib::cout << "DONE: " << hwlib::boolalpha << !radio.stereo() << hwlib::endl << hwlib::endl;
	hwlib::wait_ms(2000);

	hwlib::cout << "Setting Stereo Signal: ";
	radio.setStereo(true);
	hwlib::cout << "DONE: " << hwlib::boolalpha << radio.stereo() << hwlib::endl << hwlib::endl;
	hwlib::wait_ms(2000);

	hwlib::cout << hwlib::endl << hwlib::endl;
	hwlib::cout << "Tuning to 100.7FM with Low Side Injection: ";
	radio.setFrequency(100.7, 0);
	hwlib::cout << !radio.highSide() << hwlib::endl << hwlib::endl;
	hwlib::wait_ms(2000);

	hwlib::cout << "Tuning to 100.7FM with High Side Injection: ";
	radio.setFrequency(100.7, 1);
	hwlib::cout << radio.highSide() << hwlib::endl << hwlib::endl;
	hwlib::wait_ms(2000);

	hwlib::cout << "Muting Left Speaker: ";
	radio.setMute('l', true);
	hwlib::cout << "DONE: " << hwlib::boolalpha << radio.isMuted('l') << hwlib::endl << hwlib::endl;
	hwlib::wait_ms(2000);

	hwlib::cout << "Unmuting Left Speaker: ";
	radio.setMute('l', false);
	hwlib::cout << "DONE: " << hwlib::boolalpha << !radio.isMuted('l') << hwlib::endl << hwlib::endl;
	hwlib::wait_ms(2000);

	hwlib::cout << "Turning to Stand-By: ";
	radio.standBy();
	hwlib::cout << hwlib::boolalpha << radio.isStandBy() << hwlib::endl << hwlib::endl;
	hwlib::wait_ms(2000);

	hwlib::cout << "Turning back on: ";
	radio.standBy(false);
	hwlib::cout << hwlib::boolalpha << !radio.isStandBy() << hwlib::endl << hwlib::endl;
	hwlib::wait_ms(2000);

}
