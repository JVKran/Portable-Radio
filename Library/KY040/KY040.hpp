/// @file

#ifndef __KY040_HPP
#define __KY040_HPP

/// \brief
/// Polling KY040 Rotary Encoder Interface
/// \details
/// This is an interface that simplifies the use of the KY040 Rotary Encoder.
/// This class supports updating the position of the encoder and retrieving the
/// state of the pushbutton. Since Interrupts are not supportedthe update() 
/// function has got to be called at least every 50ms for the readings to 
/// be accurate enough for use.
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
	    KY040(hwlib::pin_in & CLK, hwlib::pin_in & DT, hwlib::pin_in & SW = hwlib::pin_in_dummy, int posCounter = 0, bool swPress = false);
	    void update();
	    int getPos();
	    void setPos(int newPos);
	    bool isPressed();
};

#endif //__KY040_HPP