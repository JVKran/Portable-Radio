#include "hwlib.hpp"
#include "radioDataSystem.hpp"


void radioDataSystem::update(const uint16_t block1, const uint16_t block2, const uint16_t block3, const uint16_t block4){
	blockA = block1;
	blockB = block2;
	blockC = block3;
	blockD = block4;
	//hwlib::cout << blockA << ", " << blockB << ", " << blockC << ", " << blockD << hwlib::endl;
}

void radioDataSystem::process(){
	auto groupType = ((blockB & 0xF000) >> 12);
	hwlib::cout << "Group Type: " << groupType << hwlib::endl;
	//auto trafficProgramm = (blockB & 0x0400);
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Country Code: " << ((blockA & 0xF000) >> 12) << hwlib::endl;			//0xFFFF where F is one nibble
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Area Coverage: " << ((blockA & 0x0F00) >> 8) << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Refrence Number: " << (blockA & 0x00FF) << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Message Group Type: ";
	if((blockB >> 11) & 1){
		hwlib::cout << "A";
	} else {
		hwlib::cout << "B";
	} 
	hwlib::cout << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Program: ";
	if((blockB >> 10) & 1){
		hwlib::cout << "Yes";
	} else {
		hwlib::cout << "No";
	} 
	hwlib::cout << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Type: " << ((blockB & 0x3E0) >> 5) << hwlib::endl;
	if((blockB >> 11) & 1){		//Message Version A
		switch(groupType){
			case 2:		//RDStext
				hwlib::cout << hwlib::left << hwlib::setw(30) << "Received RDS Text: ";
				first = ((blockC & 0xFF00) >> 8);
				second = (blockC & 0x00FF);
				third = ((blockD & 0xFF00) >> 8);
				fourth = (blockD & 0x00FF);
				charSegment0 = (blockB & 1);
				charSegment1 = (blockB >> 1) & 1;
				charSegment2 = (blockB >> 2) & 1;
				charSegment3 = (blockB >> 3) & 1;
				//bool clearScreenRequest = (blockB >> 4) & 1;
				offset = 4 * ((charSegment0 * 1) + (charSegment1 * 2) + (charSegment2 * 4) + (charSegment3 * 8));
				rdsText[offset + 1] = first;
				rdsText[offset + 2] = second;
				rdsText[offset + 3] = third;
				rdsText[offset + 4] = fourth;
				hwlib::cout << rdsText << hwlib::endl;
				break;
			case 4:
				minutes = (blockD & 0xFC0) >> 6;
				hours = ((blockD & 0xF000) >> 12) + ((blockC & 1) << 4);
				hwlib::cout << hours << ":" << minutes << hwlib::endl;
				break;
		}
	} else {		//Message Version B
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Announcement: " << ((blockB >> 5) & 1) << hwlib::endl;
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Music or Speech: " << ((blockB >> 4) & 1) << hwlib::endl;
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Station Name: ";
		first = ((blockD & 0xFF00) >> 8);
		second = (blockD & 0x00FF);
		//bool decoderIdentification = (blockB >> 3) & 1;
		charSegment0 = (blockB & 1);
		charSegment1 = (blockB >> 1) & 1;
		if(!charSegment1 && !charSegment0){
			receivedStationName[0] = first;
			receivedStationName[1] = second;
		} else if (charSegment0 && !charSegment1){
			receivedStationName[2] = first;
			receivedStationName[3] = second;
		} else if (!charSegment0 && charSegment1){
			receivedStationName[4] = first;
			receivedStationName[5] = second;
		} else {
			receivedStationName[6] = first;
			receivedStationName[7] = second;
		}
		hwlib::wait_ms(500);
		hwlib::cout << receivedStationName << hwlib::endl << hwlib::endl;
		switch(groupType){
			case 2:		//RDStext
				hwlib::cout << hwlib::left << hwlib::setw(30) << "Received RDS Text: ";
				third = ((blockD & 0xFF00) >> 8);
				fourth = (blockD & 0x00FF);
				charSegment0 = (blockB & 1);
				charSegment1 = (blockB >> 1) & 1;
				charSegment2 = (blockB >> 2) & 1;
				charSegment3 = (blockB >> 3) & 1;
				//bool clearScreenRequest = (blockB >> 4) & 1;
				offset = 2 * ((charSegment0 * 1) + (charSegment1 * 2) + (charSegment2 * 4) + (charSegment3 * 8));
				rdsText[offset + 1] = third;
				rdsText[offset + 2] = fourth;
				hwlib::cout << rdsText << hwlib::endl;
				break;
		}
	}
	hwlib::cout << hwlib::endl << hwlib::endl;
	/*
	switch(groupType){
		case 0x00;		//11
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
	*/
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

