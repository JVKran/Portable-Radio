/// @file

#ifndef __RADIO_DATA_SYSTEM_HPP
#define __RADIO_DATA_SYSTEM_HPP

/// \brief
/// Program Item Number
/// \details
/// This class stores the data provided by the PIN-Part of RDS-data. This part contains info about when the
/// broadcast started en when it is ought to stop.
/// 
///	All supported operations are:
///		- Set hours, minutes and seconds
///		- Get hours, minutes and seconds
/// 
/// ~~~~~~~~~~~~~~~{.cpp}
/// #include "hwlib.hpp"
/// #include "A24C256.hpp"
/// 
/// auto scl = target::pin_oc( target::pins::d8 );
/// auto sda = target::pin_oc( target::pins::d9 );
/// auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
/// 
/// auto radio = RDA5807(i2c_bus);
/// radio.begin();
/// hwlib::cout << "Tuning to 100.7FM: ";
/// radio.setFrequency(100.7);
/// 
/// auto pin = radio.radioData.getProgramItemNumber();
/// hwlib::cout << "Broadcast started at: day " << pin.getDay() << ", hour " << pin.getHours() << " and minute " << pin.getMinutes() << hwlib::endl;
/// ~~~~~~~~~~~~~~~
class programItemNumber{
	private:
		unsigned int day;
		unsigned int hours;
		unsigned int minutes;
	public:
		programItemNumber(unsigned int day = 0, unsigned int hours = 0, unsigned int minutes = 0);
		void setData(unsigned int day, unsigned int hours, unsigned int minutes);
		unsigned int getHours();
		unsigned int getMinutes();
		unsigned int getDay();
};

/// \brief
/// Radio Data System Data
/// \details
/// This struct contains all info needed to parse, decode, analyse and store the received data provided by the Radio
/// Data System.
/// 
///	All supported operations are:
///		- None (Just contains the Radio Data received by the Radio Data System)
///
/// Used internally to store, analyse and decode Radio Data.
struct radioDataSystemData{
	//Radio Data Received Blocks
	uint16_t blockA;
	uint16_t blockB;
	uint16_t blockC;
	uint16_t blockD;

	//Basic Info Present in all Blocks
	int groupType = 0;
	bool trafficProgram = 0;
	int offset = 0;
	int lastOffset = 0;
	int index = 0;
	int lastIndex = 0;

	//Extra info present in most Blocks
	bool stereoSignal;
	bool compressedSignal;
	bool staticProgramType; 			//Can PTY vary over time?

	//Station Name Received Characters and their Positions
	char first;
	char second;
	char third;
	char fourth;
	bool charSegment0;
	bool charSegment1;
	bool charSegment2;
	bool charSegment3;

	//Miscellaneous
	unsigned int minutes = 0;
	unsigned int hours = 0;
	unsigned int radioPaging;
	unsigned int slowLabeling;
	bool emergencyWarning;
	bool clearScreenRequest;

	//Used for Decoding and validating validity of received info
	unsigned int validI = 0;
	unsigned int newText;
	unsigned int cycles;

	//Used to store received Data
	char receivedStationName[10] = {"         "};
	char realStationName[10] = {"         "};
	char rdsText[64] = {"                                                               "};
	uint16_t status[6] = {};

	//Specific data format containing broadcast start time
	programItemNumber PIN;

	radioDataSystemData();
};

/// \brief
/// Radio Data System
/// \details
/// This class is used to handle everything that is part of the decoding, analysis and validation of the received
/// RDS-Data. 
///
/// The Radio Data System is an International Standard of Broadcasting extra meta data in the lower spectrum of FM-
/// Signals. It supports a large variety of information. From Time, to Artist, Song and Album, to Traffic Announcements.
/// In some countries ther even is a national Emergency System based on RDS. Currently RDS2 is in development. However,
/// with the rise of DAB+ it's questionable if it's even going to launch.
///
/// This class tries to support as much as possible as good as possible, and it does it pretty good. Only the channels
/// make it harder. Most of them do not completely obey the protocol.
///
/// Since almost all channels support RDS-B and almost no channels support RDS-A (even if they do it often
/// isn't complete) only RDS-B support has been tested.
///
/// All suported operations are visible below:
/// 
/// ~~~~~~~~~~~~~~~{.cpp}
/// auto scl = target::pin_oc( target::pins::d8 );
/// auto sda = target::pin_oc( target::pins::d9 );
/// auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
/// 
/// auto radio = RDA5807(i2c_bus);
/// radio.begin();
/// hwlib::cout << "Tuning to 100.7FM: ";
/// radio.setFrequency(100.7);
/// 
/// for(unsigned int i = 0; i < 8; i++){
///   	hwlib::wait_ms(5000);
///   	radio.radioData.update();
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Frequency: " << radio.getIntFrequency() << hwlib::endl;
///		hwlib::cout << hwlib::left << hwlib::setw(30) << "Station Name: " << radio.radioData.stationName() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Radio Text: " << radio.radioData.stationText() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Country Code: " << radio.radioData.getCountryCode() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Area Coverage: " << radio.radioData.getProgramArea() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Refrence Number: " << radio.radioData.getProgramRefrence() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Message Group Type: " << radio.radioData.getMessageGroupType() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Announcement: " << radio.radioData.trafficAnnouncement() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Music Playing: " << radio.radioData.currentMusic() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Clear Screen Request: " << radio.radioData.clearScreen() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Type: " << radio.radioData.getProgramType() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Static Program Type: " << radio.radioData.staticProgramType() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Stereo Broadcast: " << radio.radioData.stereo() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Compressed Broadcast: " << radio.radioData.compressed() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Emergency Warning: " << radio.radioData.emergencyWarning() << hwlib::endl;
///   	hwlib::cout << hwlib::left << hwlib::setw(30) << "Received Time: " << radio.radioData.hours() << ":" << minutes() << hwlib::endl;
///   	hwlib::cout << hwlib::endl << hwlib::endl;
///   	hwlib::wait_ms(3000);
///   	radio.seekChannel(1);
/// }
/// ~~~~~~~~~~~~~~~
class radioDataSystem{
	private:
		hwlib::i2c_bus & bus;
		const uint8_t address;
		const uint8_t indexAddress;
		const uint8_t firstReadAddress;
		void getStatus();
		unsigned int radioDataErrors(const unsigned int block);
		bool radioDataReady();
		bool radioDataSynced();
		radioDataSystemData radioData;
	public:
		radioDataSystem(hwlib::i2c_bus_bit_banged_scl_sda & bus, const uint8_t address = 0x10, const uint8_t firstReadAddress = 0x0A);
		void rawData();
		unsigned int getCountryCode();
		char* getStationName(const unsigned int dataValidity = 4);
		char* stationName();
		char* getStationText();
		char* stationText();
		bool clearScreen();
		bool trafficProgram();			//This channel features traffic announcements
		bool trafficAnnouncement();		//This channel currently talks about traffic
		unsigned int getProgramArea();
		unsigned int getProgramRefrence();
		char getMessageGroupType();
		unsigned int getProgramType();
		bool stereo();
		bool compressed();
		bool emergencyWarning();
		bool staticProgramType();
		bool currentMusic();
		unsigned int hours();
		unsigned int minutes();
		void update();
		void reset();
		programItemNumber getProgramItem();
};

#endif //__RADIO_DATA_SYSTEM_HPP