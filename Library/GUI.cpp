#include "hwlib.hpp"
#include "KY040.hpp"
#include "GUI.hpp"



signalBars::signalBars(const hwlib::xy & position, const int spacing, const int heightIncrement):
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
	window.flush();
}



//<------------------------------------------------------------------------------>




GUI::GUI(hwlib::window & window, hwlib::glcd_oled & display, KY040 & button, hwlib::terminal_from & textArea):
	window(window),
	display(display), 
	button(button),
	textArea(textArea),
	signalIndicator(signalBars(hwlib::xy(110, 10), 2, 2))
{}

void GUI::receptionStrength(const unsigned int signalStrength){
	display.clear();
	signalIndicator.print(window, signalStrength / 45);
}

void GUI::displayFrequency(float frequency){
	textArea << int(frequency) << hwlib::flush;
	display.flush();
	display.flush();
}
