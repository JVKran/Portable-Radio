#ifndef __KY040_HPP
#define __KY040_HPP

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
	    KY040(hwlib::pin_in & CLK, hwlib::pin_in & DT, hwlib::pin_in & SW, int posCounter = 0, bool swPress = false);
	    void update();
	    int getPos(){
	    	return posCounter;
	    }
	    void setPos(int newPos){
	    	posCounter = newPos;
	    }
	    bool isPressed(){
	    	return swPress;  
	    }
};

#endif //__KY040_HPP