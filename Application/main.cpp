#include "hwlib.hpp"
#include "TEA5767.hpp"
#include "GUI.hpp"
#include "KY040.hpp"

int main( void ){
    namespace target = hwlib::target;

    auto CLK = hwlib::target::pin_in( hwlib::target::pins::d36 );
	auto DT = hwlib::target::pin_in( hwlib::target::pins::d38 );
	auto SW = hwlib::target::pin_in( hwlib::target::pins::d40 );
	
	auto button = KY040(CLK, DT, SW);

    auto scl = target::pin_oc( target::pins::d8 );
    auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto radio = TEA5767(i2c_bus);
	auto oled = hwlib::glcd_oled( i2c_bus, 0x3c );

	auto frequencyArea = hwlib::window_part(oled, hwlib::xy( 0, 0 ), hwlib::xy( 30, 10 ));   
	auto font = hwlib::font_default_8x8();
	auto textArea = hwlib::terminal_from(frequencyArea, font);

   	auto window = hwlib::window_part(oled, hwlib::xy(0, 0), hwlib::xy(128, 64));
   	auto display = GUI(window, oled, button, textArea);

   	radio.setFrequency(100.7);
   	unsigned int iterations = 0;
   	int lastKnownPos = 0;
   	button.setPos(0);

   	std::array<float, 12> stations = {90.7, 92.6, 93.4, 94.7, 95.2, 97.6, 98.9, 100.1, 100.7, 101.2, 102.1, 107.4};
   	for(;;){
   		iterations++;
   		button.update();
   		if(button.getPos() != lastKnownPos){
   			lastKnownPos = button.getPos();
   			if(lastKnownPos > 11){
   				button.setPos(0);
   			} else if(lastKnownPos < 0){
   				button.setPos(11);
   			}
   			radio.setFrequency(stations[lastKnownPos]);
   			lastKnownPos = button.getPos();
   		}
   		if(iterations > 500){
   			iterations = 0;
   			display.receptionStrength(radio.signalStrength());
   			display.displayFrequency(radio.getFrequency());
   		}
   	}

}