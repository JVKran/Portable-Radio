#include "hwlib.hpp"
#include "KY040.hpp"
#include "GUI.hpp"



signalBars::signalBars(const hwlib::xy & position, const unsigned int spacing, const unsigned int heightIncrement):
	spacing(spacing),
	heightIncrement(heightIncrement),
	first(position, hwlib::xy(position.x, position.y - heightIncrement)),
	second(hwlib::xy(position.x + spacing * 1, position.y), hwlib::xy(position.x + spacing * 1, position.y - heightIncrement * 2)),
	third(hwlib::xy(position.x + spacing * 2, position.y), hwlib::xy(position.x + spacing * 2, position.y - heightIncrement * 3)),
	fourth(hwlib::xy(position.x + spacing * 3, position.y), hwlib::xy(position.x + spacing * 3, position.y - heightIncrement * 4)),
	fifth(hwlib::xy(position.x + spacing * 4, position.y), hwlib::xy(position.x + spacing * 4, position.y - heightIncrement * 5))
{}

void signalBars::print(hwlib::window & window, const unsigned int amountOfLines){
	if(amountOfLines > 0){
		first.draw(window);
	}
	if(amountOfLines > 1){
		second.draw(window);
	}
	if(amountOfLines > 2){
		third.draw(window);
	}
	if(amountOfLines > 3){
		fourth.draw(window);
	}
	if(amountOfLines > 4){
		fifth.draw(window);
	}
	//window.flush();
}



//<<<------------------------------------------------------------------------------>>>



GUI::GUI(hwlib::window & window, hwlib::glcd_oled & display, KY040 & button):
	window(window),
	display(display), 
	button(button),
	signalIndicator(signalBars(hwlib::xy(110, 10), 2, 2))
{}

/*					TEA5767 Compatible
void GUI::receptionStrength(const unsigned int signalStrength){
	display.clear();
	signalIndicator.print(window, signalStrength / 45);
}
*/

				   //RDA5807 Compatible
void GUI::receptionStrength(const unsigned int signalStrength){
	signalIndicator.print(window, signalStrength / 12);
}

void GUI::displayFrequency(const float frequency){
	auto font = hwlib::font_default_8x8();
	auto textArea = hwlib::terminal_from(window, font);
	textArea << int(frequency) << hwlib::flush;
	signalIndicator.print(window, 60 / 12);
	//window.flush();
}

void GUI::displayStationName(const char & stationName){
	auto font = hwlib::font_default_8x8();
	auto textArea = hwlib::terminal_from(window, font);
	textArea << stationName << hwlib::flush;
	signalIndicator.print(window, 60 / 12);
}

void GUI::displayMenuUpdate(const unsigned int signalStrength, const float frequency){
	if(frequency != lastFrequency || signalStrength / 12 != lastSignalStrength){
		display.clear();
		receptionStrength(signalStrength);
		displayFrequency(frequency);
		lastFrequency = frequency;
		lastSignalStrength = signalStrength / 12;
		window.flush();
	}
}
