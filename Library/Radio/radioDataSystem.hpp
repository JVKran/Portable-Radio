#ifndef __RADIO_DATA_SYSTEM_HPP
#define __RADIO_DATA_SYSTEM_HPP

class radioDataSystem{
	private:
		int groupType = 0;
		int trafficProgramm = 0;
		int offset = 0;
		int minutes = 0;
		char receivedStationName[12] = {"         \0"};
		char realStationName[12] = {"         \0"};
	public:
		radioDataSystem(){};
		void enableRDS(const bool enable);
		void processData();
};

#endif //__RADIO_DATA_SYSTEM_HPP