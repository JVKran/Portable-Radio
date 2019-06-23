#ifndef __RADIO_DATA_SYSTEM_HPP
#define __RADIO_DATA_SYSTEM_HPP

class radioDataSystem{
	private:
		char receivedStationName[] = {"         \0"};
		char realStationName[] = {"         \0"};
		int groupType = 0;
		int trafficProgramm = 0;
		int offset = 0;
		int minutes = 0;
	public:
		radioDataSystem(){};
};

#endif //__RADIO_DATA_SYSTEM_HPP