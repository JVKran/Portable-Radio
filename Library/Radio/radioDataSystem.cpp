#include "hwlib.hpp"
#include "radioDataSystem.hpp"

radioDataSystem::radioDataSystem(hwlib::i2c_bus_bit_banged_scl_sda & bus): bus(bus), radioData(radioDataSystemData()) {}

/*
void radioDataSystem::update(const uint16_t block1, const uint16_t block2, const uint16_t block3, const uint16_t block4){
	radioData.blockA = block1;
	radioData.blockB = block2;
	radioData.blockC = block3;
	radioData.blockD = block4;
	//hwlib::cout << blockA << ", " << blockB << ", " << blockC << ", " << blockD << hwlib::endl;
}
*/

void radioDataSystem::getStatus(){
	bus.write(0x11).write(0x0A);
	auto transaction = bus.read(0x11);
	for(unsigned int i = 0; i < 6; i++){
		radioData.status[i] = transaction.read_byte() << 8;
		radioData.status[i] |= transaction.read_byte();
	}
	radioData.blockA = radioData.status[2];
	radioData.blockB = radioData.status[3];
	radioData.blockC = radioData.status[4];
	radioData.blockD = radioData.status[5];
	hwlib::wait_ms(15);
}

void radioDataSystem::process(){
	getStatus();
	auto groupType = ((radioData.blockB & 0xF000) >> 12);
	hwlib::cout << "Group Type: " << groupType << hwlib::endl;
	//auto trafficProgramm = (blockB & 0x0400);
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Country Code: " << ((radioData.blockA & 0xF000) >> 12) << hwlib::endl;			//0xFFFF where F is one nibble
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Area Coverage: " << ((radioData.blockA & 0x0F00) >> 8) << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Refrence Number: " << (radioData.blockA & 0x00FF) << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Message Group Type: ";
	if((radioData.blockB >> 11) & 1){
		hwlib::cout << "A";
	} else {
		hwlib::cout << "B";
	} 
	hwlib::cout << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Program: ";
	if((radioData.blockB >> 10) & 1){
		hwlib::cout << "Yes";
	} else {
		hwlib::cout << "No";
	} 
	hwlib::cout << hwlib::endl;
	hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Type: " << ((radioData.blockB & 0x3E0) >> 5) << hwlib::endl;
	if((radioData.blockB >> 11) & 1){		//Message Version A
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Announcement: " << ((radioData.blockB >> 5) & 1) << hwlib::endl;
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Music (1) or Speech (0): " << ((radioData.blockB >> 4) & 1) << hwlib::endl;
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Station Name: ";
		radioData.first = ((radioData.blockD & 0xFF00) >> 8);
		radioData.second = (radioData.blockD & 0x00FF);
		//bool decoderIdentification = (blockB >> 3) & 1;
		radioData.charSegment0 = (radioData.blockB & 1);
		radioData.charSegment1 = (radioData.blockB >> 1) & 1;
		radioData.offset = ((radioData.charSegment0 * 1) + (radioData.charSegment1 * 2));
		radioData.receivedStationName[radioData.offset * 2] = radioData.first;
		radioData.receivedStationName[radioData.offset * 2 + 1] = radioData.second;
		if(!radioData.charSegment1 && !radioData.charSegment0){
			radioData.receivedStationName[0] = radioData.first;
			radioData.receivedStationName[1] = radioData.second;
		} else if (radioData.charSegment0 && !radioData.charSegment1){
			radioData.receivedStationName[2] = radioData.first;
			radioData.receivedStationName[3] = radioData.second;
		} else if (!radioData.charSegment0 && radioData.charSegment1){
			radioData.receivedStationName[4] = radioData.first;
			radioData.receivedStationName[5] = radioData.second;
		} else {
			radioData.receivedStationName[6] = radioData.first;
			radioData.receivedStationName[7] = radioData.second;
		}
		hwlib::wait_ms(500);
		hwlib::cout << radioData.receivedStationName << hwlib::endl << hwlib::endl;
		switch(groupType){
			case 2:		//RDStext
				hwlib::cout << hwlib::left << hwlib::setw(30) << "Received RDS Text: ";
				radioData.first = ((radioData.blockC & 0xFF00) >> 8);
				radioData.second = (radioData.blockC & 0x00FF);
				radioData.third = ((radioData.blockD & 0xFF00) >> 8);
				radioData.fourth = (radioData.blockD & 0x00FF);
				radioData.charSegment0 = (radioData.blockB & 1);
				radioData.charSegment1 = (radioData.blockB >> 1) & 1;
				radioData.charSegment2 = (radioData.blockB >> 2) & 1;
				radioData.charSegment3 = (radioData.blockB >> 3) & 1;
				//bool clearScreenRequest = (blockB >> 4) & 1;
				radioData.offset = 4 * ((radioData.charSegment0 * 1) + (radioData.charSegment1 * 2) + (radioData.charSegment2 * 4) + (radioData.charSegment3 * 8));
				radioData.rdsText[radioData.offset + 1] = radioData.first;
				radioData.rdsText[radioData.offset + 2] = radioData.second;
				radioData.rdsText[radioData.offset + 3] = radioData.third;
				radioData.rdsText[radioData.offset + 4] = radioData.fourth;
				hwlib::cout << radioData.rdsText << hwlib::endl;
				break;
			case 4:
				radioData.minutes = (radioData.blockD & 0xFC0) >> 6;
				radioData.hours = ((radioData.blockD & 0xF000) >> 12) + ((radioData.blockC & 1) << 4);
				hwlib::cout << radioData.hours << ":" << radioData.minutes << hwlib::endl;
				break;
		}
	} else {		//Message Version B
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Announcement: " << ((radioData.blockB >> 5) & 1) << hwlib::endl;
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Music or Speech: " << ((radioData.blockB >> 4) & 1) << hwlib::endl;
		hwlib::cout << hwlib::left << hwlib::setw(30) << "Station Name: ";
		radioData.first = ((radioData.blockD & 0xFF00) >> 8);
		radioData.second = (radioData.blockD & 0x00FF);
		//bool decoderIdentification = (blockB >> 3) & 1;
		radioData.charSegment0 = (radioData.blockB & 1);
		radioData.charSegment1 = (radioData.blockB >> 1) & 1;
		radioData.offset = ((radioData.charSegment0 * 1) + (radioData.charSegment1 * 2));
		radioData.receivedStationName[radioData.offset * 2] = radioData.first;
		radioData.receivedStationName[radioData.offset * 2 + 1] = radioData.second;
		if(!radioData.charSegment1 && !radioData.charSegment0){
			radioData.receivedStationName[0] = radioData.first;
			radioData.receivedStationName[1] = radioData.second;
		} else if (radioData.charSegment0 && !radioData.charSegment1){
			radioData.receivedStationName[2] = radioData.first;
			radioData.receivedStationName[3] = radioData.second;
		} else if (!radioData.charSegment0 && radioData.charSegment1){
			radioData.receivedStationName[4] = radioData.first;
			radioData.receivedStationName[5] = radioData.second;
		} else {
			radioData.receivedStationName[6] = radioData.first;
			radioData.receivedStationName[7] = radioData.second;
		}
		hwlib::wait_ms(500);
		hwlib::cout << radioData.receivedStationName << hwlib::endl << hwlib::endl;
		switch(groupType){
			case 2:		//RDStext
				hwlib::cout << hwlib::left << hwlib::setw(30) << "Received RDS Text: ";
				radioData.third = ((radioData.blockD & 0xFF00) >> 8);
				radioData.fourth = (radioData.blockD & 0x00FF);
				radioData.charSegment0 = (radioData.blockB & 1);
				radioData.charSegment1 = (radioData.blockB >> 1) & 1;
				radioData.charSegment2 = (radioData.blockB >> 2) & 1;
				radioData.charSegment3 = (radioData.blockB >> 3) & 1;
				//bool clearScreenRequest = (blockB >> 4) & 1;
				radioData.offset = 2 * ((radioData.charSegment0 * 1) + (radioData.charSegment1 * 2) + (radioData.charSegment2 * 4) + (radioData.charSegment3 * 8));
				radioData.rdsText[radioData.offset + 1] = radioData.third;
				radioData.rdsText[radioData.offset + 2] = radioData.fourth;
				hwlib::cout << radioData.rdsText << hwlib::endl;
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

char* radioDataSystem::getStationName(const unsigned int dataValidity){
	for(unsigned int i = 0; i < 8; i++){
		radioData.receivedStationName[i] = ' ';
		radioData.realStationName[i] = ' ';
	}
	getStatus();
	if((radioData.blockB >> 11) & 1){			//Version A

	} else {						//Version B
		for(unsigned int i = 0; i < dataValidity * 15; i++){
			getStatus();
			if(radioDataErrors(0) + radioDataErrors(1) <= 2){
				radioData.first = ((radioData.blockD & 0xFF00) >> 8);
				radioData.second = (radioData.blockD & 0x00FF);
				//bool decoderIdentification = (blockB >> 3) & 1;
				radioData.charSegment0 = (radioData.blockB & 1);
				radioData.charSegment1 = (radioData.blockB >> 1) & 1;
				if(radioData.first > 31 && radioData.first < 127 && radioData.second > 31 && radioData.second < 127 && radioData.first != ' ' && radioData.second != ' '){
					radioData.offset = ((radioData.charSegment0 * 1) + (radioData.charSegment1 * 2));
					radioData.receivedStationName[radioData.offset * 2] = radioData.first;
					radioData.receivedStationName[radioData.offset * 2 + 1] = radioData.second;
				} else {
					hwlib::wait_ms(20);
				}
				if(radioData.offset < radioData.lastOffset){
					if(radioData.receivedStationName == radioData.realStationName){
						radioData.cycles++;
						if(radioData.cycles >= dataValidity){			//Station Name has got to be received equal at least three times in a row.
							radioData.cycles = 0;
							break;
						}
						break;
					} else {
						for(unsigned int i = 0; i < 8; i++){
							radioData.realStationName[i] = radioData.receivedStationName[i];
						}
						radioData.cycles = 0;
						hwlib::wait_ms(20);
					}
				} else {
					radioData.lastOffset = radioData.offset;
				}
				hwlib::wait_ms(20);
			} else {
				hwlib::wait_ms(20);
			}
		}
	}
return radioData.receivedStationName;
}

char* radioDataSystem::getStationText(){
	getStatus();
	for(unsigned int i = 0; i < 100; i++){
		getStatus();
		if((radioData.blockB >> 11) & 1 && radioDataErrors(0) + radioDataErrors(1) < 1 && radioDataReady() && radioDataSynced()){		//Version A
			radioData.first = ((radioData.blockC & 0xFF00) >> 8);
			radioData.second = (radioData.blockC & 0x00FF);
			radioData.third = ((radioData.blockD & 0xFF00) >> 8);
			radioData.fourth = (radioData.blockD & 0x00FF);
			radioData.charSegment0 = (radioData.blockB & 1);
			radioData.charSegment1 = (radioData.blockB >> 1) & 1;
			radioData.charSegment2 = (radioData.blockB >> 2) & 1;
			radioData.charSegment3 = (radioData.blockB >> 3) & 1;
			radioData.offset = 4 * ((radioData.charSegment0 * 1) + (radioData.charSegment1 * 2) + (radioData.charSegment2 * 4) + (radioData.charSegment3 * 8));
			radioData.rdsText[radioData.offset + 1] = radioData.first;
			radioData.rdsText[radioData.offset + 2] = radioData.second;
			radioData.rdsText[radioData.offset + 3] = radioData.third;
			radioData.rdsText[radioData.offset + 4] = radioData.fourth;
			if(radioData.offset < radioData.lastOffset){
				break;
			} else {
				radioData.lastOffset = radioData.offset;
			}
			
		} else if (radioDataErrors(0) + radioDataErrors(1) < 1 && radioDataReady() && radioDataSynced()){
			radioData.third = ((radioData.blockD & 0xFF00) >> 8);
			radioData.fourth = (radioData.blockD & 0x00FF);
			radioData.charSegment0 = (radioData.blockB & 1);
			radioData.charSegment1 = (radioData.blockB >> 1) & 1;
			radioData.charSegment2 = (radioData.blockB >> 2) & 1;
			radioData.charSegment3 = (radioData.blockB >> 3) & 1;
			radioData.offset = 2 * ((radioData.charSegment0 * 1) + (radioData.charSegment1 * 2) + (radioData.charSegment2 * 4) + (radioData.charSegment3 * 8));
			radioData.rdsText[radioData.offset + 1] = radioData.third;
			radioData.rdsText[radioData.offset + 2] = radioData.fourth;
			if(radioData.offset < radioData.lastOffset){
				//One Text Cycle received.
				break;
			} else {
				radioData.lastOffset = radioData.offset;
			}
		}
	}
	return radioData.rdsText;
}


int radioDataSystem::radioDataErrors(const int block){
	if(block == 1){
		return (radioData.status[1] & 0x000C);
	} else {
		return (radioData.status[1] & 0x0003);
	}
}

//When a station is found or the frequency gets set, the data has to be reset for it to contain valid chars again.
void radioDataSystem::reset(){
	for(auto & element : radioData.receivedStationName){
		element = ' ';
	}
	getStatus();
}

bool radioDataSystem::radioDataReady(){
	//getStatus();
	return (radioData.status[0] >> 15) & 1;
}

bool radioDataSystem::radioDataSynced(){
	//getStatus();
	return (radioData.status[0] >> 12) & 1;
}

bool radioDataSystem::clearScreen(){
	if(radioData.clearScreenRequest){
		radioData.clearScreenRequest = false;
		return true;
	} else {
		return false;
	}
}

int radioDataSystem::getCountryCode(){
	return ((radioData.blockA & 0xF000) >> 12);
}

int radioDataSystem::getProgramArea(){
	return ((radioData.blockA & 0x0F00) >> 8);
}

int radioDataSystem::getProgramRefrence(){
	return (radioData.blockA & 0x00FF);
}

char radioDataSystem::getMessageGroupType(){
	if((radioData.blockB >> 11) & 1){
		return 'A';
	} else {
		return 'B';
	} 
}

int radioDataSystem::getProgramType(){
	return ((radioData.blockB & 0x3E0) >> 5);
}

bool radioDataSystem::currentMusic(){
	return ((radioData.blockB >> 4) & 1);
}

char* radioDataSystem::stationName(){
	return radioData.receivedStationName;
}

char* radioDataSystem::stationText(){
	return radioData.rdsText;
}

bool radioDataSystem::trafficProgram(){
	return ((radioData.blockB >> 10) & 1);
}

bool radioDataSystem::trafficAnnouncement(){
	return ((radioData.blockB >> 5) & 1);
}

int radioDataSystem::hours(){
	return radioData.hours;
}

int radioDataSystem::minutes(){
	return radioData.minutes;
}


void radioDataSystem::update(){
	getStatus();
	getStationName();
	radioData.clearScreenRequest |= (radioData.blockB >> 4) & 1;
	radioData.trafficProgram = (radioData.blockB >> 5) & 1;
	for(unsigned int i = 0; i < 4; i++){
		if(radioDataErrors(0) + radioDataErrors(1) == 0 && radioDataSynced()){
			if((radioData.blockB >> 11) & 1){		//Message Version A
				switch(((radioData.blockB & 0xF000) >> 12)){		//Group Type
					case 4:
						radioData.minutes = (radioData.blockD & 0xFC0) >> 6;
						radioData.hours = ((radioData.blockD & 0xF000) >> 12) + ((radioData.blockC & 1) << 4);
						break;
					case 2:
						getStationText();
				}
			} else {
				switch(((radioData.blockB & 0xF000) >> 12)){		//Group Type
					case 2:
						getStationText();
				}
			}
			break;
		}
	}
}