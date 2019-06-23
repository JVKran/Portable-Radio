#include "RDA5807.hpp"
#include "hwlib.hpp"


void radioDataSystem::getRDS(){
	getStatus();
	hwlib::cout << status[2] << status[3] << status[4] << status[5] << hwlib::endl;
}

void radioDataSystem::enableRDS(const bool enable){
	if(enable){
		data[2] |= (1UL << 3);
	} else {
		data[2] &= ~(1UL << 3);
	}
	setData(2);
}

bool radioDataSystem::rdsReady(){
	getStatus(0);
	return (status[0] >> 15) & 1;
}

bool radioDataSystem::rdsSync(){
	getStatus(0);
	return (status[0] >> 12) & 1;
}

void radioDataSystem::processData(){
	getStatus();
	char receivedStationName[] = {"         \0"};
	char realStationName[] = {"         \0"};
	auto groupType = ((status[3]  & 0x0800 )>> 11) | (status[3] >> 8);
	auto trafficProgramm = (status[3] & 0x0400);
	switch(groupType){
		case 0x0B:
			hwlib::cout << "Radio Text: ";
			int index = (status[3] & 0x0003);
			char first = status[5] >> 8;
			char second = status[5] & 0x00FF;
			receivedStationName[index] = first;
			receivedStationName[index + 1] = second;
			unsigned int validI = 0;
			for(unsigned int i = 0; i < 8; i++){
				if(receivedStationName[i] > 31 && receivedStationName[i] < 127){
					realStationName[validI] = receivedStationName[i];
					validI++;
				}
			}
			hwlib::cout << realStationName;
		case 0x4A:
			if(rdsErrors(1) > 0){
				hwlib::cout << "Time: "; 
				int offset = status[5] & 0x3F;
				int minutes = ((status[5] >> 6) & 0x3F);
				minutes += 60 * (((status[5] & 0x0001) << 4) | ((status[5] >> 12) & 0x0F));
				if (offset & 0x20) {
			   		minutes -= 30 * (offset & 0x1F);
			    } else {
			      	minutes += 30 * (offset & 0x1F);
				}
				hwlib::cout << minutes / 60 << ":" << minutes % 60 << hwlib::endl;
			} else {
				hwlib::cout << "Time, but too much errors..." << hwlib::endl;
			}
		default:
			break;
	}
	/*
	hwlib::cout << "Program Identification: " << hwlib::endl << hwlib::boolalpha;
	hwlib::cout << "PI: " << status[2] << hwlib::endl;
	hwlib::cout << "Message Group Type B? (1 y, 0 n): " << ((status[3] >> 11) & 1) << hwlib::endl;
	hwlib::cout << "Group Type: ";
	if((status[3] >> 12) & 4){
		if(rdsErrors(1) > 0){
			hwlib::cout << "Time: "; 
			int offset = status[5] & 0x3F;
			int minutes = ((status[5] >> 6) & 0x3F);
			minutes += 60 * (((status[5] & 0x0001) << 4) | ((status[5] >> 12) & 0x0F));
			if (offset & 0x20) {
		   		minutes -= 30 * (offset & 0x1F);
		    } else {
		      	minutes += 30 * (offset & 0x1F);
			}
			hwlib::cout << minutes / 60 << ":" << minutes % 60 << hwlib::endl;
		} else {
			hwlib::cout << "Time, but too much errors..." << hwlib::endl;
		}
	} else if((status[3] >> 12) & 2){
		hwlib::cout << "Radio Text: ";
		int index = (status[3] & 0x0003);
		char first = status[5] >> 8;
		char second = status[5] & 0x00FF;
		receivedStationName[index] = first;
		receivedStationName[index + 1] = second;
		unsigned int validI = 0;
		for(unsigned int i = 0; i < 8; i++){
			if(receivedStationName[i] > 31 && receivedStationName[i] < 127){
				realStationName[validI] = receivedStationName[i];
				validI++;
			}
		}
		hwlib::cout << realStationName;
	} else if((status[3] >> 12) & 0){
		hwlib::cout << "Basic Tuning Info";
	} else {
		hwlib::cout << "Other";
	}
	hwlib::cout << hwlib::endl;
	hwlib::cout << "Traffic Program: " << ((status[3] >> 10) & 1) << hwlib::endl;
	hwlib::cout << "Program Type: " << ((status[3] & 0x3E0) >> 5) << hwlib::endl;
	hwlib::cout << hwlib::endl;

	hwlib::cout << "Payload: " << char(status[4] >> 8) << char(status[4] & 0xFF) << ", " << char(status[5] >> 8) << char(status[5] & 0xFF) << hwlib::endl;
	*/
}

int radioDataSystem::rdsErrors(const int block){
	getStatus(block);
	if(block == 1){
		return (status[1] & 0x000C);
	} else {
		return (status[1] & 0x0003);
	}
}
