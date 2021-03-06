/// @file

#include "hwlib.hpp"
#include "KY040.hpp"
#include "RDA5807.hpp"
#include "GUI.hpp"
#include "DS3231.hpp"


/// \brief
/// Constructor
/// \details
/// This constructor has no mandatory parameters. The location defaults to hwlib::xy(0, 0), height to 5 and width
/// also to 5. However, these can be given to make use of the object.
batteryBars::batteryBars(const hwlib::xy & position, const unsigned int height, const unsigned int width):
	left(position, hwlib::xy(position.x, position.y + height)),
	right(hwlib::xy(position.x + width, position.y), hwlib::xy(position.x + width, position.y + height + 1)),
	top(hwlib::xy(position.x, position.y + height), hwlib::xy(position.x + width, position.y + height)),
	bottom(position, hwlib::xy(position.x + width, position.y)),
	tip(hwlib::xy(position.x - 1, position.y + 1), hwlib::xy(position.x -1, position.y + height - 1)),
	height(height),
	width(width),
	position(position)
{}


/// \brief
/// Print Bars
/// \details
/// This function prints the amount of bars relative to the given percentage of voltage.
void batteryBars::print(hwlib::window & window, const unsigned int percentage){
	left.draw(window);
	right.draw(window);
	top.draw(window);
	bottom.draw(window);
	tip.draw(window);
	for(unsigned int i = 0; i <= (width * 10) / 10 * percentage / 100; i++){
		hwlib::line line(hwlib::xy( position.x + width - i, position.y), hwlib::xy(  position.x + width - i, position.y + height ));
	   	line.draw(window);
	}
	window.flush();
}

//<<<--------------------------------------------------------------------------->>>

/// \brief
/// Constructor
/// \details
/// This constructor has no mandatory parameters. The location defaults to hwlib::xy(0, 0), height to 5 and width
/// to 15. However, these can be given to make use of the object.
signalBars::signalBars(const hwlib::xy & position, const unsigned int spacing, const unsigned int heightIncrement):
	spacing(spacing),
	heightIncrement(heightIncrement),
	first(position, hwlib::xy(position.x, position.y - heightIncrement)),
	second(hwlib::xy(position.x + spacing * 1, position.y), hwlib::xy(position.x + spacing * 1, position.y - heightIncrement * 2)),
	third(hwlib::xy(position.x + spacing * 2, position.y), hwlib::xy(position.x + spacing * 2, position.y - heightIncrement * 3)),
	fourth(hwlib::xy(position.x + spacing * 3, position.y), hwlib::xy(position.x + spacing * 3, position.y - heightIncrement * 4)),
	fifth(hwlib::xy(position.x + spacing * 4, position.y), hwlib::xy(position.x + spacing * 4, position.y - heightIncrement * 5))
{}

/// \brief
/// Print Bars
/// \details
/// This constructor function prints the given amount of lines; from 0 to 5.
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


/// \brief
/// Constructor
/// \details
/// This constructor has a lot of mandatory parameters. However, they all seem very logical when you look at
/// the first part of the main.cpp from this application.
GUI::GUI(hwlib::window & window, hwlib::glcd_oled & display, KY040 & button, 
	hwlib::terminal_from & stereoField, 
	hwlib::window & signalWindow, 
	hwlib::window & batteryWindow,
	hwlib::terminal_from & frequencyField,
	hwlib::terminal_from & menuField,
	hwlib::terminal_from & settingsField,
	hwlib::terminal_from & stationField
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
	menuField(menuField),
	settingsField(settingsField),
	stationField(stationField)
{
	display.clear();
}

/*					TEA5767 Compatible
void GUI::receptionStrength(const unsigned int signalStrength){
	display.clear();
	signalIndicator.print(window, signalStrength / 45);
}
*/

/// \brief
/// Print Reception Strenght
/// \details
/// This function calls print() from signalBars.
				   //RDA5807 Compatible
void GUI::receptionStrength(const unsigned int signalStrength){
	signalWindow.clear();
	signalIndicator.print(window, signalStrength / 12);
}

/// \brief
/// Print Battery Percentage
/// \details
/// This function calls print() from batteryBars.
void GUI::batteryPercentage(const unsigned int voltage){
	batteryWindow.clear();
	batteryIndicator.print(window, (voltage - 32) * 10);			//4.2V = 100%, 3.2V = 0%
}

/// \brief
/// Display Stereo
/// \details
/// This function prints "ST" or "MN" when respectively stereo or mono signal is received
/// . It prints this in the given StereoField.
void GUI::displayStereo(const bool stereo){
	if(stereo){
		stereoField << "\f" << "ST" << hwlib::flush;
	} else {
		stereoField << "\f" << "MN" << hwlib::flush;
	}
}

/// \brief
/// Display Station Name
/// \details
/// This function prints the given station name in the textArea field and then also prints
/// the signalindicator since this could have changed after retrieving RDS station Name.
void GUI::displayStationName(const char & stationName){
	auto font = hwlib::font_default_8x8();
	auto textArea = hwlib::terminal_from(window, font);
	textArea << hwlib::right << stationName << hwlib::flush;
	signalIndicator.print(window, 60 / 12);
}

/// \brief
/// Display Menu
/// \details
/// This function prints the current menu the user is in; can be one of
/// auto, manual or presets.
void GUI::displayMenuArea(const unsigned int menuArea){
	if(menuArea == 0){
		menuField << "\f" << "      Auto " << hwlib::flush;
	} else if (menuArea == 1){
		menuField << "\f" << "     Manual" << hwlib::flush;
	} else if (menuArea == 2){
		menuField << "\f" << "     Presets" << hwlib::flush;
	}
}

/// \brief
/// Display Tuned Frequency
/// \details
/// This function prints the currently tuned frequency in the frequencyField.
void GUI::displayFrequency(const unsigned int frequency, const bool change){
	if(change){
		frequencyField << "\f" << '<' << frequency / 10 << '.' << (frequency % 10) << '>' << hwlib::flush;
	} else {
		frequencyField << "\f" << ' ' << frequency / 10 << '.' << (frequency % 10) << ' ' << hwlib::flush;
	}
}

/// \brief
/// Update GUI
/// \details
/// This function updates all values that have changed.
void GUI::displayMenuUpdate(const unsigned int signalStrength, const float frequency, const bool change, const unsigned int voltage,const bool stereo, const unsigned int menuArea, Radio & radio, const bool showRadioDataStationName, const char*  stationName, const bool curMute, const dateData & date, const bool force){
	if(frequency != lastFrequency || change != lastChange || force){
		displayFrequency(frequency, change);
		displayMenuArea(menuArea);
		displayStereo(stereo);
		lastStereo = stereo;
		lastFrequency = frequency;
		lastChange = change;
	}
	if(signalStrength / 12 != lastSignalStrength || force){
		receptionStrength(signalStrength);
		lastSignalStrength = signalStrength / 12;
	}
	if(voltage != lastVoltage || force){
		batteryPercentage(voltage);
		lastVoltage = voltage;
	}
	if(stereo != lastStereo || force){
		displayStereo(stereo);
		lastStereo = stereo;
	}
	if(menuArea != lastMenuArea && menuArea < 3){
		displayMenuArea(menuArea);
		lastMenuArea = menuArea;
	}
	if((menuArea == 3 && menuArea != lastMenuArea) || radio.bassBoosted() != lastBoost){
		menuField << hwlib::boolalpha;
		menuField << "\f" << " Bass ";
		if(radio.bassBoosted()){
			menuField << "Boosted";
		} else {
			menuField << "Unboosted";
		}
		menuField << hwlib::flush;
		lastBoost = radio.bassBoosted();
		lastMenuArea = menuArea;
	}
	if((menuArea == 4 && menuArea != lastMenuArea) || curMute != lastMute){
		menuField << hwlib::boolalpha;
		if(curMute){
			menuField << "\f" << "   Bluetooth";
		} else {
			menuField << "\f" << "     Radio";
		}
		menuField << hwlib::flush;
		lastMute = curMute;
		lastMenuArea = menuArea;
	}
	if((menuArea == 5 && menuArea != lastMenuArea) || radio.radioDataEnabled() != lastEnabled){
		menuField << hwlib::boolalpha;
		if(radio.radioDataEnabled()){
			menuField << "\f" << "  RDS enabled";
		} else {
			menuField << "\f" << "  RDS disabled";
		}
		menuField << hwlib::flush;
		lastEnabled = radio.radioDataEnabled();
		lastMenuArea = menuArea;
	}
	if((menuArea == 6) || showRadioDataStationName != lastStationRDS){
		menuField << hwlib::boolalpha;
		if(showRadioDataStationName){
			menuField << "\f" << " 	  RDS Name";
		} else {
			menuField << "\f" << "   Preset Name";
		}
		menuField << hwlib::flush;
		lastStationRDS = showRadioDataStationName;
		lastMenuArea = menuArea;
	}
	if(menuArea == 7){
		menuField << "\f" << "    " << date << hwlib::flush;
		lastMenuArea = menuArea;
	}
	displayStereo(stereo);
	lastStereo = stereo;
	stationField << "\f" << stationName << hwlib::flush;
	for(unsigned int i = 0; i < 9; i++){
		lastStationName[i] = stationName[i];
	}
}

/// \brief
/// Display Settings
/// \details
/// This function prints all given settings. Is interchangable between RDA5807 and TEA5767 since they have
/// the same base class. Currently not used.
void GUI::showSettings(KY040 & button, Radio & radio, unsigned int & menuArea){
	button.update();
	auto pos = button.getPos();
	hwlib::cout << hwlib::boolalpha;
	if(pos == 0){
		settingsField << 	"\f" << "-> Vol: " << radio.getVolume() <<
					"\n" << "   Bass: " << radio.bassBoosted() <<
				"\t0002" << "   Mute: " << radio.isMuted() <<
				"\t0003" << "   RDS:  " << radio.radioDataEnabled() <<
				"\t0004" << "   Back" <<
				hwlib::flush;
	} else if (pos == 1){
		settingsField << 	"\f" << "   Vol: " << radio.getVolume() <<
					"\n" << "-> Bass: " << radio.bassBoosted() <<
				"\t0002" << "   Mute: " << radio.isMuted() <<
				"\t0003" << "   RDS:  " << radio.radioDataEnabled() <<
				"\t0004" << "   Back" <<
				hwlib::flush;
	} else if (pos == 2){
		settingsField << 	"\f" << "   Vol: " << radio.getVolume() <<
					"\n" << "   Bass: " << radio.bassBoosted() <<
				"\t0002" << "-> Mute: " << radio.isMuted() <<
				"\t0003" << "   RDS:  " << radio.radioDataEnabled() <<
				"\t0004" << "   Back" <<
				hwlib::flush;
	} else if (pos == 3){
		settingsField << 	"\f" << "   Vol: " << radio.getVolume() <<
					"\n" << "   Bass: " << radio.bassBoosted() <<
				"\t0002" << "   Mute: " << radio.isMuted() <<
				"\t0003" << "-> RDS:  " << radio.radioDataEnabled() <<
				"\t0004" << "   Back" <<
				hwlib::flush;
	} else {
		settingsField << 	"\f" << "   Vol: " << radio.getVolume() <<
					"\n" << "   Bass: " << radio.bassBoosted() <<
				"\t0002" << "   Mute: " << radio.isMuted() <<
				"\t0003" << "   RDS:  " << radio.radioDataEnabled() <<
				"\t0004" << "-> Back" <<
				hwlib::flush;
	}
	if(button.isPressed()){
		menuArea = 0;
		button.setPos(0);
	}
}
