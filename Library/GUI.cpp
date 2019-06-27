#include "hwlib.hpp"
#include "KY040.hpp"
#include "GUI.hpp"


batteryBars::batteryBars(const hwlib::xy & position, const unsigned int height, const unsigned int width):
	left(position, hwlib::xy(position.x, position.y + height)),
	right(hwlib::xy(position.x + width, position.y + 1), hwlib::xy(position.x + width, position.y + height)),
	top(hwlib::xy(position.x, position.y + height), hwlib::xy(position.x + width, position.y + height)),
	bottom(position, hwlib::xy(position.x + width + 2, position.y)),
	tip(hwlib::xy(position.x - 1, position.y + 1), hwlib::xy(position.x -1, position.y + height - 1)),
	height(height),
	width(width),
	position(position)
{}



void batteryBars::print(hwlib::window & window, const unsigned int percentage){
	left.draw(window);
	right.draw(window);
	top.draw(window);
	bottom.draw(window);
	tip.draw(window);
	for(unsigned int i = 0; i <= (width * 10) / 10 * percentage / 100; i++){
		hwlib::line line(hwlib::xy( position.x + width - i + 1, position.y), hwlib::xy(  position.x + width - i + 1, position.y + height ));
	   	line.draw(window);
	}
	window.flush();
}

//<<<--------------------------------------------------------------------------->>>


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
	window.flush();
}



//<<<------------------------------------------------------------------------------>>>



GUI::GUI(hwlib::window & window, hwlib::glcd_oled & display, KY040 & button, 
	hwlib::terminal_from & stereoField, 
	hwlib::window & signalWindow, 
	hwlib::window & batteryWindow,
	hwlib::terminal_from & frequencyField,
	hwlib::terminal_from & menuField
):
	window(window),
	display(display), 
	button(button),
	signalIndicator(signalBars(hwlib::xy(110, 8), 2, 2)),
	batteryIndicator(batteryBars(hwlib::xy(90, 2))),
	stereoField(stereoField),
	signalWindow(signalWindow),
	batteryWindow(batteryWindow),
	frequencyField(frequencyField),
	menuField(menuField)
{
	display.clear();
}

/*					TEA5767 Compatible
void GUI::receptionStrength(const unsigned int signalStrength){
	display.clear();
	signalIndicator.print(window, signalStrength / 45);
}
*/

				   //RDA5807 Compatible
void GUI::receptionStrength(const unsigned int signalStrength){
	signalWindow.clear();
	signalIndicator.print(window, signalStrength / 12);
}

void GUI::batteryPercentage(const unsigned int voltage){
	batteryWindow.clear();
	batteryIndicator.print(window, (voltage - 32) * 10);			//4.2V = 100%, 3.2V = 0%
}

void GUI::displayStereo(const bool stereo){
	if(stereo){
		stereoField << "\f" << "ST" << hwlib::flush;
	} else {
		stereoField << "\f" << "MN" << hwlib::flush;
	}
}

void GUI::displayStationName(const char & stationName){
	auto font = hwlib::font_default_8x8();
	auto textArea = hwlib::terminal_from(window, font);
	textArea << stationName << hwlib::flush;
	signalIndicator.print(window, 60 / 12);
}

void GUI::displayMenuArea(const unsigned int menuArea){
	if(menuArea == 0){
		menuField << "Automatic" << hwlib::flush;
	} else if (menuArea == 1){
		menuField << "Manually" << hwlib::flush;
	}
}

void GUI::displayFrequency(const unsigned int frequency, const bool change){
	if(change){
		frequencyField << "\f" << '<' << frequency / 10 << '.' << (frequency % 10) << '>' << hwlib::flush;
	} else {
		frequencyField << "\f" << ' ' << frequency / 10 << '.' << (frequency % 10) << ' ' << hwlib::flush;
	}
}

void GUI::displayMenuUpdate(const unsigned int signalStrength, const float frequency, const bool change, const unsigned int voltage,const bool stereo, const unsigned int menuArea){
	if(frequency != lastFrequency || change != lastChange){
		displayFrequency(frequency, change);
		lastFrequency = frequency;
		lastChange = change;
	}
	if(signalStrength / 12 != lastSignalStrength){
		receptionStrength(signalStrength);
		lastSignalStrength = signalStrength / 12;
	}
	if(voltage != lastVoltage){
		batteryPercentage(voltage);
		lastVoltage = voltage;
	}
	if(stereo != lastStereo){
		displayStereo(stereo);
		lastStereo = stereo;
	}
	if(menuArea != lastMenuArea){
		displayMenuArea(menuArea);
		lastMenuArea = menuArea;
	}
}

void GUI::showSettings(){

}
