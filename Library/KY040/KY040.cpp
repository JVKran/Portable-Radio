/// @file

#include "hwlib.hpp"
#include "KY040.hpp"

/// \brief
/// Constuctor
/// \details
/// This constructor has two mandatory parameters; the CLK and DT pin. Without these pins
/// it is impossible to determine the position of the rotary encoder. The SW doesn't have
/// to be connected; it then always returns 0. 
KY040::KY040(hwlib::pin_in & CLK, hwlib::pin_in & DT, hwlib::pin_in & SW, const int posCounter, const bool swPress): CLK(CLK), DT(DT), SW(SW), posCounter(posCounter), swPress(swPress) {
	CLK.refresh();
	lastPos = CLK.read();
	curPos = lastPos;
}

/// \brief
/// Update readings
/// \details
/// This function determines the position of the encoder and checks wether the button is
/// pressed. Since Rotary Encoders are completely time-based, this function has got to be
/// called frequently enough for the values to be updated correctly (~10ms / ~80ms). The
/// Rotary Encoder outputs a grey-code and this function tries to interpret that outputted
/// code. It's very basic though.
void KY040::update(){
	CLK.refresh();
	curPos = CLK.read();
	if(curPos != lastPos){
		DT.refresh();
		if(DT.read() != curPos){
			posCounter++;
		} else {
			posCounter--;
		}
	}
	SW.refresh();
	swPress = !SW.read();
	lastPos = curPos;
}

/// \brief
/// Get Position
/// \details
/// This function returns the current position of the encoder. For the value to be recent,
/// the update() function has got to be called often enough. Positions can be both, 
/// negative (counter clockwise) and positive (clockwise).
int KY040::getPos(){
	return posCounter;
}

/// \brief
/// Set Position
/// \details
/// This function can be used to set the position to a desired value. This way a new zero-
/// point can be selected. Positions can be both, negative (counter clockwise) and positive (clockwise).
void KY040::setPos(const int newPos){
	posCounter = newPos;
}

/// \brief
/// Get Button State
/// \details
/// This function returns true if the button is pressed or false when it is not pressed. For
/// the value to be accurate and recent, the update() function has got te be called often
/// enough.
bool KY040::isPressed(){
	return swPress;  
}

/// \brief
/// Test Correct Functioning
/// \details
/// This function performs a quick test to figure out if the chip is working.
bool KY040::testCorrectFunctioning(){
	hwlib::cout << "Testprocedure KY040. Press the button." << hwlib::endl;
	for(;;){
		update();
		if(isPressed()){
			hwlib::cout << "Button pressed!" << hwlib::endl;
			while(isPressed()){
				update();
				hwlib::wait_ms(5);
			}
			hwlib::cout << hwlib::endl;
			break;
		}
		hwlib::wait_ms(5);
	}

	hwlib::cout << "Keep the button pressed for 5 seconds." << hwlib::endl;
	update();
	while(!isPressed()){
		update();
		hwlib::wait_ms(5);
	}
	if(isPressed()){
		hwlib::cout << "Button pressed!" << hwlib::endl;
		for(unsigned int i = 0; i < 20; i++){
		update();
			if(i == 18){
				hwlib::cout << hwlib::endl << "Kept button pressed for 5 seconds!" << hwlib::endl << hwlib::endl;
				break;
			}
			if(!isPressed()){
				hwlib::cout << "Test Failed!" << hwlib::endl << hwlib::endl;
				break;
			}
			hwlib::cout << '-';
			hwlib::wait_ms(250);
		}
	}
	hwlib::wait_ms(5);


	hwlib::cout << "Turn Rotary Encoder Clockwise." << hwlib::endl;
	update();
	int lastPos = getPos();
	for(;;){
		update();
		if(getPos() > lastPos){
			hwlib::cout << "Turned Clockwise!" << hwlib::endl << hwlib::endl;
			break;
		}
		hwlib::wait_ms(5);
	}

	hwlib::cout << "Turn Rotary Encoder 5x Clockwise." << hwlib::endl;
	update();
	lastPos = getPos();
	for(;;){
		update();
		if(getPos() > lastPos + 4){
			hwlib::cout << "Turned 5x Clockwise!" << hwlib::endl << hwlib::endl;
			break;
		}
		hwlib::wait_ms(5);
	}

	return true;		//If fails, this part isn't even reached.

}