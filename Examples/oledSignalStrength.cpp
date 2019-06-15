#include "hwlib.hpp"
#include "TEA5767.hpp"

int main( void ){
    namespace target = hwlib::target; 
    
    auto scl = target::pin_oc( target::pins::d8 );
    auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

   	auto oled = hwlib::glcd_oled( i2c_bus, 0x3c );
   	auto font = hwlib::font_default_8x8();
   	auto display = hwlib::terminal_from(oled, font);  
	auto radio = TEA5767(i2c_bus);

	oled.clear();
   	

	radio.setStereo(true);
	radio.audioSettings(true, true, true);
	radio.setFrequency(100.7);
	for(;;){
		radio.setFrequency(100.7);
		display << "\f" << int(radio.getFrequency()) << "          " << radio.signalStrength() << hwlib::flush;
		hwlib::wait_ms(10000);
		radio.setFrequency(102.1);
		hwlib::wait_ms(1000);
		display << "\f" << int(radio.getFrequency()) << "          " << radio.signalStrength() << hwlib::flush;
		hwlib::wait_ms(10000);
	}
}

