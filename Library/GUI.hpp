#ifndef __GUI_HPP
#define __GUI_HPP

#include "KY040.hpp"

class signalBars{
	private:
		unsigned int spacing;
		unsigned int heightIncrement;
		unsigned int lastAmountOfLines;
		hwlib::line first, second, third, fourth, fifth;
	public:
		signalBars(const hwlib::xy & position, const unsigned int spacing = 5, const unsigned int heightIncrement = 5);
		void print(hwlib::window & window, const unsigned int amountOfLines);
};

class GUI{
	private:
		hwlib::window & window;
		hwlib::glcd_oled & display;
		KY040 & button;
		signalBars signalIndicator;
		unsigned int lastSignalStrength = 0;
		unsigned int lastFrequency = 0;
	public:
		GUI(hwlib::window & window_part, hwlib::glcd_oled & display, KY040 & button);
		void receptionStrength(const unsigned int signalStrength);
		void displayFrequency(const float frequency);
		void displayStationName(const char & stationName);
		void displayMenuUpdate(const unsigned int signalStrength, const float frequency);
};


#endif //__GUI_HPP