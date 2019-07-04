/// @file

#include "hwlib.hpp"
#include "KY040.hpp"

/// \brief
/// Test
/// \details
/// This program tests ALL functionality of the Rotary Encoder (tested with KY040 and EC11).
int main( void ){
    namespace target = hwlib::target; 

	auto CLK = hwlib::target::pin_in( hwlib::target::pins::d36 );
	auto DT = hwlib::target::pin_in( hwlib::target::pins::d38 );
	auto SW = hwlib::target::pin_in( hwlib::target::pins::d40 );
	auto button = KY040(CLK, DT, SW);

	hwlib::wait_ms(2000);

	hwlib::cout << "Testprocedure KY040. Press the button." << hwlib::endl;
	for(;;){
		button.update();
		if(button.isPressed()){
			hwlib::cout << "Button pressed!" << hwlib::endl;
			while(button.isPressed()){
				button.update();
				hwlib::wait_ms(5);
			}
			hwlib::cout << hwlib::endl;
			break;
		}
		hwlib::wait_ms(5);
	}

	hwlib::cout << "Keep the button pressed for 5 seconds." << hwlib::endl;
	button.update();
	while(!button.isPressed()){
		button.update();
		hwlib::wait_ms(5);
	}
	if(button.isPressed()){
		hwlib::cout << "Button pressed!" << hwlib::endl;
		for(unsigned int i = 0; i < 20; i++){
		button.update();
			if(i == 18){
				hwlib::cout << hwlib::endl << "Kept button pressed for 5 seconds!" << hwlib::endl << hwlib::endl;
				break;
			}
			if(!button.isPressed()){
				hwlib::cout << "Test Failed!" << hwlib::endl << hwlib::endl;
				break;
			}
			hwlib::cout << '-';
			hwlib::wait_ms(250);
		}
	}
	hwlib::wait_ms(5);


	hwlib::cout << "Turn Rotary Encoder Clockwise." << hwlib::endl;
	button.update();
	int lastPos = button.getPos();
	for(;;){
		button.update();
		if(button.getPos() > lastPos){
			hwlib::cout << "Turned Clockwise!" << hwlib::endl << hwlib::endl;
			break;
		}
		hwlib::wait_ms(5);
	}

	hwlib::cout << "Turn Rotary Encoder 5x Clockwise." << hwlib::endl;
	button.update();
	lastPos = button.getPos();
	for(;;){
		button.update();
		if(button.getPos() > lastPos + 4){
			hwlib::cout << "Turned 5x Clockwise!" << hwlib::endl << hwlib::endl;
			break;
		}
		hwlib::wait_ms(5);
	}

	hwlib::cout << "Turn Rotary Encoder Counter-Clockwise." << hwlib::endl;
	button.update();
	lastPos = button.getPos();
	for(;;){
		button.update();
		if(button.getPos() < lastPos){
			hwlib::cout << "Turned Counter-Clockwise!" << hwlib::endl << hwlib::endl;
			break;
		}
		hwlib::wait_ms(5);
	}

	hwlib::cout << "Turn Rotary Encoder 5x Counter-Clockwise." << hwlib::endl;
	button.update();
	lastPos = button.getPos();
	for(;;){
		button.update();
		if(button.getPos() + 4 < lastPos){
			hwlib::cout << "Turned 5x Counter-Clockwise!" << hwlib::endl << hwlib::endl;
			break;
		}
		hwlib::wait_ms(5);
	}

	hwlib::cout << hwlib::endl << "Test Succeeded" << hwlib::endl;
}

