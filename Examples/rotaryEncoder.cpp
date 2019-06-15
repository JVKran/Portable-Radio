#include "hwlib.hpp"
#include "TEA5767.hpp"
#include "KY040.hpp"

int main( void ){
    namespace target = hwlib::target; 

	auto CLK = hwlib::target::pin_in( hwlib::target::pins::d22 );
	auto DT = hwlib::target::pin_in( hwlib::target::pins::d24 );
	auto SW = hwlib::target::pin_in( hwlib::target::pins::d26 );
	auto button = KY040(CLK, DT, SW);

	for(;;){
		button.update();
		hwlib::cout << button.getPos() << hwlib::endl;
		while(button.isPressed()){
			button.update();
			hwlib::wait_ms(5);
			hwlib::cout << "Pressed" << hwlib::endl;
		}
	}
}

