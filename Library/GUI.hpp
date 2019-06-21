#ifndef __GUI_HPP
#define __GUI_HPP

#include "KY040.hpp"

class signalBars{
	private:
		int spacing;
		int heightIncrement;
		hwlib::line first, second, third, fourth, fifth;
	public:
		signalBars(const hwlib::xy & position, const int spacing = 5, const int heightIncrement = 5);
		void print(hwlib::window & window, const unsigned int amountOfLines);
};

class GUI{
	private:
		hwlib::window & window;
		hwlib::glcd_oled & display;
		KY040 & button;
		hwlib::terminal & textArea;
		signalBars signalIndicator;
	public:
		GUI(hwlib::window & window_part, hwlib::glcd_oled & display, KY040 & button, hwlib::terminal_from & textArea);
		void receptionStrength(const unsigned int signalStrength);
		void displayFrequency(float frequency);
};


#endif //__GUI_HPP