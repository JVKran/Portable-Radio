#include "hwlib.hpp"
#include "radioDataSystem.hpp"

radioDataSystem::radioDataSystem(hwlib::i2c_bus_bit_banged_scl_sda & bus): bus(bus), radioData(radioDataSystemData()) {}

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

void radioDataSystem::rawData(){
	getStatus();
	update();
    //hwlib::cout << hwlib::left << hwlib::setw(30) << "Frequency: " << radio.getIntFrequency() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Station Name: " << stationName() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Radio Text: " << stationText() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Country Code: " << getCountryCode() << hwlib::endl;      //0xFFFF where F is one nibble
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Area Coverage: " << getProgramArea() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Refrence Number: " << getProgramRefrence() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Message Group Type: " << getMessageGroupType() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Announcement: " << trafficAnnouncement() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Music Playing: " << currentMusic() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Clear Screen Request: " << clearScreen() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Type: " << getProgramType() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Static Program Type: " << staticProgramType() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Stereo Broadcast: " << stereo() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Compressed Broadcast: " << compressed() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Emergency Warning: " << emergencyWarning() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Received Time: " << hours() << ":" << minutes() << hwlib::endl;
    hwlib::cout << hwlib::endl << hwlib::endl;
}

char* radioDataSystem::getStationName(const unsigned int dataValidity){
	for(unsigned int i = 0; i < 8; i++){
		radioData.receivedStationName[i] = ' ';
		radioData.realStationName[i] = ' ';
	}
	getStatus();					//Version B
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
	return radioData.receivedStationName;
}

char* radioDataSystem::getStationText(){
	getStatus();
	for(unsigned int i = 0; i < 30; i++){
		getStatus();
		//Message Version A
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
			radioData.rdsText[radioData.offset] = radioData.first;
			radioData.rdsText[radioData.offset + 1] = radioData.second;
			radioData.rdsText[radioData.offset + 2] = radioData.third;
			radioData.rdsText[radioData.offset + 3] = radioData.fourth;
			if(radioData.offset < radioData.lastOffset){
				break;
			} else {
				radioData.lastOffset = radioData.offset;
			}
		//Message Version B
		} else if (radioDataErrors(0) + radioDataErrors(1) < 1 && radioDataReady() && radioDataSynced()){
			radioData.third = ((radioData.blockD & 0xFF00) >> 8);
			radioData.fourth = (radioData.blockD & 0x00FF);
			radioData.charSegment0 = (radioData.blockB & 1);
			radioData.charSegment1 = (radioData.blockB >> 1) & 1;
			radioData.charSegment2 = (radioData.blockB >> 2) & 1;
			radioData.charSegment3 = (radioData.blockB >> 3) & 1;
			radioData.offset = 2 * ((radioData.charSegment0 * 1) + (radioData.charSegment1 * 2) + (radioData.charSegment2 * 4) + (radioData.charSegment3 * 8));
			radioData.rdsText[radioData.offset] = radioData.third;
			radioData.rdsText[radioData.offset + 1] = radioData.fourth;
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

void radioDataSystem::update(){
	getStatus();
	getStationName();
	radioData.clearScreenRequest |= (radioData.blockB >> 4) & 1;
	radioData.trafficProgram = (radioData.blockB >> 5) & 1;
	for(unsigned int i = 0; i < 4; i++){
		if(radioDataErrors(0) + radioDataErrors(1) == 0 && radioDataSynced()){
			if((radioData.blockB >> 11) & 1){		//Message Version A
				switch(((radioData.blockB & 0xF000) >> 12)){		//Group Type
					case 0:		//Basic Tuning Information
						radioData.offset = 3 - ((radioData.charSegment0 * 1) + (radioData.charSegment1 * 2));
						if(radioData.offset == 0){
							if((radioData.blockB >> 2) & 1){
								radioData.stereoSignal = true;
							} else {
								radioData.stereoSignal = false;
							}
						} else if (radioData.offset == 2){
							if((radioData.blockB >> 2) & 1){
								radioData.compressedSignal = true;
							} else {
									radioData.compressedSignal = false;
							}
						} else if (radioData.offset == 3){
							if((radioData.blockB >> 2) & 1){
								radioData.staticProgramType = true;
							} else {
								radioData.staticProgramType = false;
							}
						}
						break;
					case 1:
						radioData.PIN.setData((radioData.blockD & 0xF800) >> 11, (radioData.blockD & 0x07C0) >> 6, radioData.blockD & 0x003F);
						radioData.radioPaging = (radioData.blockB & 0x001C) >> 2;
						radioData.slowLabeling = (radioData.blockC & 0x7000) >> 12;
						if(radioData.slowLabeling == 7){
							radioData.emergencyWarning = true;
						} else {
							radioData.emergencyWarning = false;
						}
					case 2:
						getStationText();
						break;
					case 4:
						radioData.minutes = (radioData.blockD & 0xFC0) >> 6;
						radioData.hours = ((radioData.blockD & 0xF000) >> 12) + ((radioData.blockC & 1) << 4);
						break;
				}
			} else {
				switch(((radioData.blockB & 0xF000) >> 12)){		//Group Type
					case 0:		//Basic Tuning Information
						radioData.offset = 3 - ((radioData.charSegment0 * 1) + (radioData.charSegment1 * 2));
						if(radioData.offset == 0){
							if((radioData.blockB >> 2) & 1){
								radioData.stereoSignal = true;
							} else {
								radioData.stereoSignal = false;
							}
						} else if (radioData.offset == 2){
							if((radioData.blockB >> 2) & 1){
								radioData.compressedSignal = true;
							} else {
									radioData.compressedSignal = false;
							}
						} else if (radioData.offset == 3){
							if((radioData.blockB >> 2) & 1){
								radioData.staticProgramType = true;
							} else {
								radioData.staticProgramType = false;
							}
						}
						break;
					case 1:
						radioData.PIN.setData((radioData.blockD & 0xF800) >> 11, (radioData.blockD & 0x07C0) >> 6, radioData.blockD & 0x003F);
						break;
					case 2:
						getStationText();
						break;
					default:
						break;
				}
			}
			break;
		}
	}
}

bool radioDataSystem::stereo(){
	return radioData.stereoSignal;
}

bool radioDataSystem::compressed(){
	return radioData.compressedSignal;
}

bool radioDataSystem::staticProgramType(){
	return radioData.staticProgramType;
}

programItemNumber radioDataSystem::getProgramItem(){
	return radioData.PIN;
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
	for(auto & element : radioData.rdsText){
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

bool radioDataSystem::emergencyWarning(){
	return radioData.emergencyWarning;
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
	return &radioData.receivedStationName[0];
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

programItemNumber::programItemNumber(int day, int hours, int minutes): 
	day(day), 
	hours(hours), 
	minutes(minutes)
{}

void programItemNumber::setData(int day, int hours, int minutes){
	day = day;
	hours = hours;
	minutes = minutes;
}
