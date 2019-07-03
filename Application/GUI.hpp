/// @file

#ifndef __GUI_HPP
#define __GUI_HPP

#include "KY040.hpp"
#include "RDA5807.hpp"
#include "DS3231.hpp"

/// \brief
/// Signaltrenght Bars
/// \details
/// This is a class which makes it possible to represent a signalstrength through bars. They try to look
/// like signalbars known from mobile phones. They are completely customizable; the position, width, height
/// amount of bars and spacing between bars.
/// 
/// ~~~~~~~~~~~~~~~{.cpp}
/// auto radio = RDA5807(i2c_bus);
///	auto signalWindow = hwlib::window_part(oled, hwlib::xy(108, 0), hwlib::xy(128, 15));
/// auto signalIndicator = signalBars(hwlib::xy(110, 8), 2, 2);
/// signalIndicator.print(window, radio.signalStrenght() / 12);
///	}
/// ~~~~~~~~~~~~~~~
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

/// \brief
/// Battery Bars
/// \details
/// This is a class which makes it possible to represent the amount of energy left through bars. It tries
/// to resemble the battery icon on mobile phones.
/// 
/// ~~~~~~~~~~~~~~~{.cpp}
///	auto batteryWindow = hwlib::window_part(oled, hwlib::xy(90, 0), hwlib::xy(105, 10));
/// batteryIndicator = batteryBars(hwlib::xy(90, 2));
/// auto battery = hwlib::target::pin_adc(0);
/// auto voltage = battery.read();
/// batteryIndicator.print(window, (voltage - 32) * 10));
///	}
/// ~~~~~~~~~~~~~~~
class batteryBars{
	private:
		hwlib::line left, right, top, bottom, tip;
		const unsigned int height;
		const unsigned int width;
		const hwlib::xy position;
	public:
		batteryBars(const hwlib::xy & position = hwlib::xy(0, 0), const unsigned int height = 5, const unsigned int width = 15);
		void print(hwlib::window & window, const unsigned int percentage);
};

/// \brief
/// Graphical User Interface
/// \details
/// This is a class which makes it possible to display the entire GUI with the given values.
/// ~~~~~~~~~~~~~~~{.cpp}
///	display.displayMenuUpdate(radio.signalStrength(), radio.getFrequency() * 10, inPressedArea, battery.read(), radio.stereoReception(), menuArea, radio, showRadioDataStationName, (char*)&stationName[0], curMute, clock.getDate());
/// 
///	}
/// ~~~~~~~~~~~~~~~
class GUI{
	private:
		hwlib::window & window;
		hwlib::glcd_oled & display;
		KY040 & button;
		signalBars signalIndicator;
		batteryBars batteryIndicator;
		unsigned int lastSignalStrength = 0;
		unsigned int lastFrequency = 0;
		bool lastChange = false;
		unsigned int lastVoltage = 0;
		bool lastStereo = false;
		unsigned int lastMenuArea = 0;
		unsigned int lastVolume = 0;
		bool lastBoost = false;
		bool lastMute = false;
		bool lastEnabled = false;
		bool lastStationRDS = false;
		hwlib::terminal & stereoField;
		hwlib::window & signalWindow;
		hwlib::window & batteryWindow;
		hwlib::terminal & frequencyField;
		hwlib::terminal & menuField;
		hwlib::terminal & settingsField;
		hwlib::terminal & stationField;
		char lastStationName[10] = {"         "};
	public:
		GUI(hwlib::window & window_part, hwlib::glcd_oled & display, KY040 & button, 
			hwlib::terminal_from & stereoField, 
			hwlib::window & signalWindow, 
			hwlib::window & batteryWindow,
			hwlib::terminal_from & frequencyField,
			hwlib::terminal_from & menuField,
			hwlib::terminal_from & settingsField,
			hwlib::terminal_from & stationField
		);
		void receptionStrength(const unsigned int signalStrength);
		void batteryPercentage(const unsigned int voltage);
		void displayStereo(const bool stereo);
		void displayStationName(const char & stationName);
		void displayFrequency(const unsigned int frequency, const bool change);
		void displayMenuArea(const unsigned int menuArea);
		void displayMenuUpdate(const unsigned int signalStrength, const float frequency, const bool change, const unsigned int voltage,const bool stereo, const unsigned int menuArea, Radio & radio, const bool showRadioDataStationName, const char* stationName, const bool curMute, const dateData & date, const bool force = false);
		void displaySettings();
		void showSettings(KY040 & button, Radio & radio, unsigned int & menuArea);
};


#endif //__GUI_HPP