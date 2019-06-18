/// @file

#include "hwlib.hpp"
#include "KY040.hpp"

/// \brief
/// Constuctor
/// \details
/// This constructor has two mandatory parameters; the CLK and DT pin. Without these pins
/// it is impossible to determine the position of the rotary encoder. The SW doesn't have
/// to be connected; it then always returns 0. 
KY040::KY040(hwlib::pin_in & CLK, hwlib::pin_in & DT, hwlib::pin_in & SW, int posCounter, bool swPress): CLK(CLK), DT(DT), SW(SW), posCounter(posCounter), swPress(swPress) {
	CLK.refresh();
	lastPos = CLK.read();
	curPos = lastPos;
}

/// \brief
/// Update readings
/// \details
/// This function determines the position of the encoder and checks wether the button is
/// pressed. Since Rotary Encoders are completely time-based, this function has got te be
/// called frequently enough for the values to be updated correctly (~10ms / ~80ms).
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
/// the update() function has got to be called often enough.
int KY040::getPos(){
	return posCounter;
}

/// \brief
/// Set Position
/// \details
/// This function can be used to set the position to a desired value. This way a new zero-
/// point can be selected.
void KY040::setPos(int newPos){
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