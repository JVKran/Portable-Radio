#ifndef __RADIO_DATA_SYSTEM_HPP
#define __RADIO_DATA_SYSTEM_HPP

class radioDataSystem{
	private:
		uint16_t blockA;
		uint16_t blockB;
		uint16_t blockC;
		uint16_t blockD;
		uint16_t blockE;
		int groupType = 0;
		int trafficProgramm = 0;
		int offset = 0;
		int minutes = 0;
		int index = 0;
		char first;
		char second;
		unsigned int validI = 0;
		unsigned int newText;
		char receivedStationName[12] = {"         \0"};
		char realStationName[12] = {"         \0"};
	public:
		radioDataSystem(){};
		void process();
		void update(const uint16_t block1, const uint16_t block2, const uint16_t block3, const uint16_t block4);
};

#endif //__RADIO_DATA_SYSTEM_HPP