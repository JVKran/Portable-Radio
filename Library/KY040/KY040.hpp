/// @file

#ifndef __KY040_HPP
#define __KY040_HPP

/// \brief
/// Polling KY040 Rotary Encoder Interface
/// \details
/// This is an interface that simplifies the use of the KY040 Rotary Encoder.
/// This class supports updating the position of the encoder and retrieving the
/// state of the pushbutton. Since Interrupts are not supported the update() 
/// function has got to be called at least every 50ms for the readings to 
/// be accurate enough for use.
/// 
///	All supported operations are:
///		- Get Position
///		- Set Position
///		- Get Button State
///		- Update Position and Button
///
/// ~~~~~~~~~~~~~~~{.cpp}
/// auto CLK = hwlib::target::pin_in( hwlib::target::pins::d22 );
/// auto DT = hwlib::target::pin_in( hwlib::target::pins::d24 );
/// auto SW = hwlib::target::pin_in( hwlib::target::pins::d26 );
/// auto button = KY040(CLK, DT, SW);
/// 
/// for(;;){
/// 	button.update();
/// 	hwlib::cout << button.getPos() << hwlib::endl;
/// 	while(button.isPressed()){
/// 		button.update();
/// 		hwlib::wait_ms(5);
/// 		hwlib::cout << "Pressed" << hwlib::endl;
/// 	}
/// }
/// ~~~~~~~~~~~~~~~
class KY040{
	private:
	   	hwlib::pin_in & CLK;
	    hwlib::pin_in & DT;
	    hwlib::pin_in & SW;
	    int posCounter;
	    bool swPress;
	    int lastPos;
	    int curPos;
  	public:
	    KY040(hwlib::pin_in & CLK, hwlib::pin_in & DT, hwlib::pin_in & SW = hwlib::pin_in_dummy, const int posCounter = 0, const bool swPress = false);
	    void update();
	    int getPos();
	    void setPos(const int newPos);
	    bool isPressed();

	    bool testCorrectFunctioning();
};

#endif //__KY040_HPP