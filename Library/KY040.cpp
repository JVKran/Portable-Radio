#include "hwlib.hpp"
#include "KY040.hpp"

KY040::KY040(hwlib::pin_in & CLK, hwlib::pin_in & DT, hwlib::pin_in & SW, int posCounter, bool swPress): CLK(CLK), DT(DT), SW(SW), posCounter(posCounter), swPress(swPress) {
	CLK.refresh();
	lastPos = CLK.read();
	curPos = lastPos;
}

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