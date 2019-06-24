#include "hwlib.hpp"
#include "radioDataSystem.hpp"


void radioDataSystem::update(const uint16_t block1, const uint16_t block2, const uint16_t block3, const uint16_t block4){
	blockA = block1;
	blockB = block2;
	blockC = block3;
	blockD= block4;
}

void radioDataSystem::process(){
	auto groupType = ((blockB & 0xF000) >> 12);
	hwlib::cout << "Group Type: " << groupType << hwlib::endl;
	//auto trafficProgramm = (blockB & 0x0400);
	switch(groupType){
		case 0x0B:		//11
			hwlib::cout << "Radio Text: ";
			index = (blockB & 0x0003);
			first = blockD >> 8;
			second = blockD & 0x00FF;
			receivedStationName[index] = first;
			receivedStationName[index + 1] = second;
			for(unsigned int i = 0; i < 8; i++){
				if(receivedStationName[i] > 31 && receivedStationName[i] < 127){
					realStationName[validI] = receivedStationName[i];
					validI++;
				}
			}
			hwlib::cout << realStationName;
			break;
		case 0x2A:		//42
			newText = (blockB & 0x0010);
			index = 4 * (blockB & 0x00F);

		case 0x4A:		//74
			hwlib::cout << "Time: "; 
			offset = blockD & 0x3F;
			minutes = ((blockD >> 6) & 0x3F);
			minutes += 60 * (((blockD & 0x0001) << 4) | ((blockD >> 12) & 0x0F));
			if (offset & 0x20) {
				minutes -= 30 * (offset & 0x1F);
			} else {
			  	minutes += 30 * (offset & 0x1F);
			}
			hwlib::cout << minutes / 60 + 7 << ":" << minutes % 60 << hwlib::endl;
			break;
		default:
			break;
	}
	/*
	hwlib::cout << "Program Identification: " << hwlib::endl << hwlib::boolalpha;
	hwlib::cout << "PI: " << blockA << hwlib::endl;
	hwlib::cout << "Message Group Type B? (1 y, 0 n): " << ((blockB >> 11) & 1) << hwlib::endl;
	hwlib::cout << "Group Type: ";
	if((blockB >> 12) & 4){
		if(rdsErrors(1) > 0){
			hwlib::cout << "Time: "; 
			int offset = blockD & 0x3F;
			int minutes = ((blockD >> 6) & 0x3F);
			minutes += 60 * (((blockD & 0x0001) << 4) | ((blockD >> 12) & 0x0F));
			if (offset & 0x20) {
		   		minutes -= 30 * (offset & 0x1F);
		    } else {
		      	minutes += 30 * (offset & 0x1F);
			}
			hwlib::cout << minutes / 60 << ":" << minutes % 60 << hwlib::endl;
		} else {
			hwlib::cout << "Time, but too much errors..." << hwlib::endl;
		}
	} else if((blockB >> 12) & 2){
		hwlib::cout << "Radio Text: ";
		int index = (blockB & 0x0003);
		char first = blockD >> 8;
		char second = blockD & 0x00FF;
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
	} else if((blockB >> 12) & 0){
		hwlib::cout << "Basic Tuning Info";
	} else {
		hwlib::cout << "Other";
	}
	hwlib::cout << hwlib::endl;
	hwlib::cout << "Traffic Program: " << ((blockB >> 10) & 1) << hwlib::endl;
	hwlib::cout << "Program Type: " << ((blockB & 0x3E0) >> 5) << hwlib::endl;
	hwlib::cout << hwlib::endl;

	hwlib::cout << "Payload: " << char(blockC >> 8) << char(blockC & 0xFF) << ", " << char(blockD >> 8) << char(blockD & 0xFF) << hwlib::endl;
	*/
}

