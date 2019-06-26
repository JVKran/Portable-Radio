#ifndef __RADIO_DATA_SYSTEM_HPP
#define __RADIO_DATA_SYSTEM_HPP

class programItemNumber{
	private:
		unsigned int day;
		unsigned int hours;
		unsigned int minutes;
	public:
		programItemNumber(unsigned int day = 0, unsigned int hours = 0, unsigned int minutes = 0);
		void setData(unsigned int day, unsigned int hours, unsigned int minutes);
		int getHours();
		int getMins();
};

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
	unsigned int minutes;
	unsigned int hours;
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