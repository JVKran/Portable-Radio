#include "hwlib.hpp"
#include "radioDataSystem.hpp"

radioDataSystem::radioDataSystem(hwlib::i2c_bus_bit_banged_scl_sda & bus): bus(bus), data(radioData()) {}

void radioDataSystem::update(const uint16_t block1, const uint16_t block2, const uint16_t block3, const uint16_t block4){
	data.blockA = block1;
	data.blockB = block2;
	data.blockC = block3;
	data.blockD = block4;
	//hwlib::cout << blockA << ", " << blockB << ", " << blockC << ", " << blockD << hwlib::endl;
}

int radioDataSystem::radioDataErrors(const int block){
	if(block == 1){
		return (data.status[1] & 0x000C);
	} else {
		return (data.status[1] & 0x0003);
	}
}

void radioDataSystem::getStatus(){
	bus.write(0x11).write(0x0A);
	auto transaction = bus.read(0x11);
	for(unsigned int i = 0; i < 6; i++){
		data.status[i] = transaction.read_byte() << 8;
		data.status[i] |= transaction.read_byte();
	}
	data.blockA = data.status[2];
	data.blockB = data.status[3];
	data.blockC = data.status[4];
	data.blockD = data.status[5];
	hwlib::wait_ms(15);
}

void radioDataSystem::process(){
	getStatus();
	auto groupType = ((data.blockB & 0xF000) >> 12);
	hwlib::cout << "Group Type: " << groupType << hwlib::endl;
	//auto trafficProgramm = (blockB & 0x0400);
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Country Code: " << ((data.blockA & 0xF000) >> 12) << hwlib::endl;			//0xFFFF where F is one nibble
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Area Coverage: " << ((data.blockA & 0x0F00) >> 8) << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Refrence Number: " << (data.blockA & 0x00FF) << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Message Group Type: ";
	if((data.blockB >> 11) & 1){
		hwlib::cout << "A";
	} else {
		hwlib::cout << "B";
	} 
	hwlib::cout << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Program: ";
	if((data.blockB >> 10) & 1){
		hwlib::cout << "Yes";
	} else {
		hwlib::cout << "No";
	} 
	hwlib::cout << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Type: " << ((data.blockB & 0x3E0) >> 5) << hwlib::endl;
	if((data.blockB >> 11) & 1){		//Message Version A
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Announcement: " << ((data.blockB >> 5) & 1) << hwlib::endl;
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Music (1) or Speech (0): " << ((data.blockB >> 4) & 1) << hwlib::endl;
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Station Name: ";
		data.first = ((data.blockD & 0xFF00) >> 8);
		data.second = (data.blockD & 0x00FF);
		//bool decoderIdentification = (blockB >> 3) & 1;
		data.charSegment0 = (data.blockB & 1);
		data.charSegment1 = (data.blockB >> 1) & 1;
		data.offset = ((data.charSegment0 * 1) + (data.charSegment1 * 2));
		data.receivedStationName[data.offset * 2] = data.first;
		data.receivedStationName[data.offset * 2 + 1] = data.second;
		if(!data.charSegment1 && !data.charSegment0){
			data.receivedStationName[0] = data.first;
			data.receivedStationName[1] = data.second;
		} else if (data.charSegment0 && !data.charSegment1){
			data.receivedStationName[2] = data.first;
			data.receivedStationName[3] = data.second;
		} else if (!data.charSegment0 && data.charSegment1){
			data.receivedStationName[4] = data.first;
			data.receivedStationName[5] = data.second;
		} else {
			data.receivedStationName[6] = data.first;
			data.receivedStationName[7] = data.second;
		}
		hwlib::wait_ms(500);
		hwlib::cout << data.receivedStationName << hwlib::endl << hwlib::endl;
		switch(groupType){
			case 2:		//RDStext
				hwlib::cout << hwlib::left << hwlib::setw(30) << "Received RDS Text: ";
				data.first = ((data.blockC & 0xFF00) >> 8);
				data.second = (data.blockC & 0x00FF);
				data.third = ((data.blockD & 0xFF00) >> 8);
				data.fourth = (data.blockD & 0x00FF);
				data.charSegment0 = (data.blockB & 1);
				data.charSegment1 = (data.blockB >> 1) & 1;
				data.charSegment2 = (data.blockB >> 2) & 1;
				data.charSegment3 = (data.blockB >> 3) & 1;
				//bool clearScreenRequest = (blockB >> 4) & 1;
				data.offset = 4 * ((data.charSegment0 * 1) + (data.charSegment1 * 2) + (data.charSegment2 * 4) + (data.charSegment3 * 8));
				data.rdsText[data.offset + 1] = data.first;
				data.rdsText[data.offset + 2] = data.second;
				data.rdsText[data.offset + 3] = data.third;
				data.rdsText[data.offset + 4] = data.fourth;
				hwlib::cout << data.rdsText << hwlib::endl;
				break;
			case 4:
				data.minutes = (data.blockD & 0xFC0) >> 6;
				data.hours = ((data.blockD & 0xF000) >> 12) + ((data.blockC & 1) << 4);
				hwlib::cout << data.hours << ":" << data.minutes << hwlib::endl;
				break;
		}
	} else {		//Message Version B
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Announcement: " << ((data.blockB >> 5) & 1) << hwlib::endl;
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Music or Speech: " << ((data.blockB >> 4) & 1) << hwlib::endl;
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Station Name: ";
		data.first = ((data.blockD & 0xFF00) >> 8);
		data.second = (data.blockD & 0x00FF);
		//bool decoderIdentification = (blockB >> 3) & 1;
		data.charSegment0 = (data.blockB & 1);
		data.charSegment1 = (data.blockB >> 1) & 1;
		data.offset = ((data.charSegment0 * 1) + (data.charSegment1 * 2));
		data.receivedStationName[data.offset * 2] = data.first;
		data.receivedStationName[data.offset * 2 + 1] = data.second;
		if(!data.charSegment1 && !data.charSegment0){
			data.receivedStationName[0] = data.first;
			data.receivedStationName[1] = data.second;
		} else if (data.charSegment0 && !data.charSegment1){
			data.receivedStationName[2] = data.first;
			data.receivedStationName[3] = data.second;
		} else if (!data.charSegment0 && data.charSegment1){
			data.receivedStationName[4] = data.first;
			data.receivedStationName[5] = data.second;
		} else {
			data.receivedStationName[6] = data.first;
			data.receivedStationName[7] = data.second;
		}
		hwlib::wait_ms(500);
		hwlib::cout << data.receivedStationName << hwlib::endl << hwlib::endl;
		switch(groupType){
			case 2:		//RDStext
				hwlib::cout << hwlib::left << hwlib::setw(30) << "Received RDS Text: ";
				data.third = ((data.blockD & 0xFF00) >> 8);
				data.fourth = (data.blockD & 0x00FF);
				data.charSegment0 = (data.blockB & 1);
				data.charSegment1 = (data.blockB >> 1) & 1;
				data.charSegment2 = (data.blockB >> 2) & 1;
				data.charSegment3 = (data.blockB >> 3) & 1;
				//bool clearScreenRequest = (blockB >> 4) & 1;
				data.offset = 2 * ((data.charSegment0 * 1) + (data.charSegment1 * 2) + (data.charSegment2 * 4) + (data.charSegment3 * 8));
				data.rdsText[data.offset + 1] = data.third;
				data.rdsText[data.offset + 2] = data.fourth;
				hwlib::cout << data.rdsText << hwlib::endl;
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
		if(radioDataErrors(1) > 0){
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

int radioDataSystem::getCountryCode(){
	return ((data.blockA & 0xF000) >> 12);
}

void radioDataSystem::reset(){
	for(auto & element : data.receivedStationName){
		element = ' ';
	}
}

char* radioDataSystem::getStationName(const unsigned int dataValidity){
	/*
	for(unsigned int i = 0; i < 8; i++){
		//receivedStationName[i] = ' ';
		realStationName[i] = ' ';
	}
	*/
	getStatus();
	if((data.blockB >> 11) & 1){			//Version A

	} else {						//Version B
		data.first = ((data.blockD & 0xFF00) >> 8);
		data.second = (data.blockD & 0x00FF);
		//bool decoderIdentification = (blockB >> 3) & 1;
		data.charSegment0 = (data.blockB & 1);
		data.charSegment1 = (data.blockB >> 1) & 1;
		hwlib::wait_ms(10);
		getStatus();
		//Check if the values received are the same.
		if(((data.blockD & 0xFF00) >> 8) == data.first && (data.blockD & 0x00FF) == data.second && (data.blockB & 1) == data.charSegment0 && ((data.blockB >> 1) & 1) == data.charSegment1 && radioDataErrors(0) + radioDataErrors(1) == 0){
			if(data.first > 31 && data.first < 127 && data.second > 31 && data.second < 127 && data.first != ' ' && data.second != ' '){
				data.offset = ((data.charSegment0 * 1) + (data.charSegment1 * 2));
				data.receivedStationName[data.offset * 2] = data.first;
				data.receivedStationName[data.offset * 2 + 1] = data.second;
			}
		}
	}
	return data.receivedStationName;
}

char* radioDataSystem::getStationText(){
	getStatus();
	for(unsigned int i = 0; i < 100; i++){
		getStatus();
		if((data.blockB >> 11) & 1 && radioDataErrors(0) + radioDataErrors(1) < 1 && radioDataReady() && radioDataSynced()){		//Version A
			data.first = ((data.blockC & 0xFF00) >> 8);
			data.second = (data.blockC & 0x00FF);
			data.third = ((data.blockD & 0xFF00) >> 8);
			data.fourth = (data.blockD & 0x00FF);
			data.charSegment0 = (data.blockB & 1);
			data.charSegment1 = (data.blockB >> 1) & 1;
			data.charSegment2 = (data.blockB >> 2) & 1;
			data.charSegment3 = (data.blockB >> 3) & 1;
			//bool clearScreenRequest = (blockB >> 4) & 1;
			data.offset = 4 * ((data.charSegment0 * 1) + (data.charSegment1 * 2) + (data.charSegment2 * 4) + (data.charSegment3 * 8));
			data.rdsText[data.offset + 1] = data.first;
			data.rdsText[data.offset + 2] = data.second;
			data.rdsText[data.offset + 3] = data.third;
			data.rdsText[data.offset + 4] = data.fourth;
			if(data.offset < data.lastOffset){
				break;
			} else {
				data.lastOffset = data.offset;
			}
			
		} else if (radioDataErrors(0) + radioDataErrors(1) < 1 && radioDataReady() && radioDataSynced()){
			data.third = ((data.blockD & 0xFF00) >> 8);
			data.fourth = (data.blockD & 0x00FF);
			data.charSegment0 = (data.blockB & 1);
			data.charSegment1 = (data.blockB >> 1) & 1;
			data.charSegment2 = (data.blockB >> 2) & 1;
			data.charSegment3 = (data.blockB >> 3) & 1;
			//bool clearScreenRequest = (blockB >> 4) & 1;
			data.offset = 2 * ((data.charSegment0 * 1) + (data.charSegment1 * 2) + (data.charSegment2 * 4) + (data.charSegment3 * 8));
			data.rdsText[data.offset + 1] = data.third;
			data.rdsText[data.offset + 2] = data.fourth;
			if(data.offset < data.lastOffset){
				//One Text Cycle received.
				break;
			} else {
				data.lastOffset = data.offset;
			}
		}
	}
	return data.rdsText;
}

bool radioDataSystem::radioDataReady(){
	//getStatus();
	return (data.status[0] >> 15) & 1;
}

bool radioDataSystem::radioDataSynced(){
	//getStatus();
	return (data.status[0] >> 12) & 1;
}
char* radioDataSystem::stationName(){
	return data.receivedStationName;
}

char* radioDataSystem::stationText(){
	return data.rdsText;
}

void radioDataSystem::update(){
	getStatus();
	getStationName();
}