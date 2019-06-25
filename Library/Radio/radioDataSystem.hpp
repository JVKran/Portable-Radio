#ifndef __RADIO_DATA_SYSTEM_HPP
#define __RADIO_DATA_SYSTEM_HPP

class programItemNumber{
	private:
		int day;
		int hours;
		int minutes;
	public:
		programItemNumber(int day = 0, int hours = 0, int minutes = 0);
		void setData(int day, int hours, int minutes);
		int getHours();
		int getMins();
};

struct radioDataSystemData{
	char first;
	char second;
	char third;
	char fourth;
	int minutes;
	int hours;
	int radioPaging;
	int slowLabeling;
	bool emergencyWarning;
	bool charSegment0;
	bool charSegment1;
	bool charSegment2;
	bool charSegment3;
	bool clearScreenRequest;
	uint16_t blockA;
	uint16_t blockB;
	uint16_t blockC;
	uint16_t blockD;
	uint16_t blockE;
	int groupType = 0;
	bool trafficProgram = 0;
	int offset = 0;
	int lastOffset = 0;
	int index = 0;
	int lastIndex = 0;
	unsigned int validI = 0;
	unsigned int newText;
	bool stereoSignal;
	bool compressedSignal;
	bool staticProgramType; 			//Can PTY vary over time?
	char receivedStationName[10] = {"         "};
	char realStationName[10] = {"         "};
	char rdsText[64] = {"                                                               "};
	uint16_t status[6] = {0x0000, 0x0000};
	unsigned int cycles;
	programItemNumber PIN;
	radioDataSystemData(): PIN(programItemNumber()) {}
};

class radioDataSystem{
	private:
		hwlib::i2c_bus & bus;
		void getStatus();
		int radioDataErrors(const int block);
		bool radioDataReady();
		bool radioDataSynced();
		radioDataSystemData radioData;
	public:
		radioDataSystem(hwlib::i2c_bus_bit_banged_scl_sda & bus);
		void rawData();
		int getCountryCode();
		char* getStationName(const unsigned int dataValidity = 4);
		char* stationName();
		char* getStationText();
		char* stationText();
		bool clearScreen();
		bool trafficProgram();			//This channel features traffic announcements
		bool trafficAnnouncement();		//This channel currently talks about traffic
		int getProgramArea();
		int getProgramRefrence();
		char getMessageGroupType();
		int getProgramType();
		bool currentMusic();
		int hours();
		int minutes();
		void update();
		void reset();
};

#endif //__RADIO_DATA_SYSTEM_HPP