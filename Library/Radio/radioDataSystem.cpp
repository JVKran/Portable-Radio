/// @file

#include "hwlib.hpp"
#include "radioDataSystem.hpp"

/// \brief
/// Constructor
/// \details
/// This constructor has one mandatory parameter; the I2C bus. Since this function is specifically made for the RDA58XX
/// Series, it is tuned towards the RDA58XX with corresponding addresses.
radioDataSystem::radioDataSystem(hwlib::i2c_bus_bit_banged_scl_sda & bus, const uint8_t address, const uint8_t firstReadAddress): 
	bus(bus),
	address(address),
	indexAddress(address + 1),
	firstReadAddress(firstReadAddress),
	radioData(radioDataSystemData())
{}

/// \brief
/// Get Radio Data
/// \details
/// This function retrieves the Radio Data Blocks and General Statuses from the Registers of the RDA58XX.
/// This class also stores the standard Registers of The RDA58XX since they contain information about validity,
/// quality and availability of Radio Data.
void radioDataSystem::getStatus(){
	bus.write(indexAddress).write(firstReadAddress);
	auto transaction = bus.read(indexAddress);
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

/// \brief
/// Print Raw Radio Data
/// \details
/// This function prints the received Radio Data in a clear way whith basic information. Really usefull to
/// debug and test the decoding of the received data.
void radioDataSystem::rawData(){
	getStatus();
	update();
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

/// \brief
/// Get Station Name
/// \details
/// This function retrieves the Data Blocks at least 15 times to get a valid Station Name. Every time data gets
/// received, it contains only two characters. Next to some other information, which is gathered in the update() function
/// , it also contains the index of the received characters. Wether or not 1 bit gets received correctly can determine
/// if the Station Name is right or wrong. A mistake is make easy, even when the received Station Name is compared 5 times.
/// It has one optional parameter (which defaults to 4) that defines how many times the data blocks have to be received (value * 15) and
/// how many times the Station Name has to be the same for it to be valid.
/// If there are more than 2 errors, the program will wait 20ms to wait for new data blocks to be received.
char* radioDataSystem::getStationName(const unsigned int dataValidity){
	for(unsigned int i = 0; i < 8; i++){
		radioData.receivedStationName[i] = ' ';
		radioData.realStationName[i] = ' ';
	}
	getStatus();
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

/// \brief
/// Get Station Text
/// \details
/// This function retrieves the data blocks containing Radio Text. That is only the case when the group type is equal
/// to 2. That's why this function can only be called from inside update(), which checks what group type the received 
/// Radio Data is. The difference between Message Version A and B is that Version A contains 4 Characters while Version B
/// only contains 2 Characters. The amount of integers that define the index is the same, but the index is calculated differently.
/// This is all according to the Radio Data System Standard.
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

/// \brief
/// Update Radio Data
/// \details
/// This function Retrieves, Analyses and Decodes the received Radio Data Blocks. Since getStatus() gets called, which updates
/// both the status array and the Blocks, information that is at the same index and at the same block for both version A and B,
/// can just be asked through simple functions. But since Version A and B have different names, they support different data and 
/// even if they contain the same info; not at the same index in the same block. For most data there has got to be made a difference
/// between verion A and B. That's where this function comes in. Firstly, the data is refreshed. Secondly, the Station Name is 
/// determined through getStationName(). Third, the Message Version is determined. And last, based on the Group Type,
/// we can interpret the received Data, which is then stored and updated in radioData of type radioDataSystemData.
void radioDataSystem::update(){
	getStatus();
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

/// \brief
/// Stereo Signal
/// \details
/// This function returns true if the Broadcasted Signal is Stereo according to the Broadcaster.
bool radioDataSystem::stereo(){
	return radioData.stereoSignal;
}

/// \brief
/// Compressed Signal
/// \details
/// This function returns true if the Broadcasted Signal is Compressed, or false when it is uncompressed.
bool radioDataSystem::compressed(){
	return radioData.compressedSignal;
}

/// \brief
/// Static or Dynamic Program Type
/// \details
/// This function returns if the Program Type of this statin can vary over time. False if otherwise.
bool radioDataSystem::staticProgramType(){
	return radioData.staticProgramType;
}

/// \brief
/// Get Program Item
/// \details
/// This function returns a Program Item containing start Time and Date of the current Broadcast.
programItemNumber radioDataSystem::getProgramItem(){
	return radioData.PIN;
}

/// \brief
/// Amount of Radio Data Errors
/// \details
/// This function returns the amount of errors in the received Radio Data Blocks. This is a good indication
/// about the quality of the Radio Data, but can be wrong since this data is also send across RDS.
unsigned int radioDataSystem::radioDataErrors(const unsigned int block){
	if(block == 1){
		return (radioData.status[1] & 0x000C);
	} else {
		return (radioData.status[1] & 0x0003);
	}
}

/// \brief
/// Reset Stored Radio Data
/// \details
/// This function resets the Radio Data. This way the process of retrieving Station Names and Texts is optimised
/// after a change of Frequency. Has got to be called after a frequency change for good performance.
void radioDataSystem::reset(){
	for(auto & element : radioData.receivedStationName){
		element = ' ';
	}
	for(auto & element : radioData.rdsText){
		element = ' ';
	}
	getStatus();
}

/// \brief
/// New Radio Data Ready
/// \details
/// This function returns true if new Radio Data is available. In practice, this always is the case when tuned to 
/// a channel with good audio quality.
bool radioDataSystem::radioDataReady(){
	//getStatus();
	return (radioData.status[0] >> 15) & 1;
}

/// \brief
/// Radio Data Synchronised
/// \details
/// This function returns true if the chip is in sync with the Radio Data Blocks that arrive. When this is the case,
/// the data has a smaller chance to contain errors.
bool radioDataSystem::radioDataSynced(){
	//getStatus();
	return (radioData.status[0] >> 12) & 1;
}

/// \brief
/// Emergency Warning
/// \details
/// This function returns true if an Emergency Warning has been send. Only available in a small number of countries.
bool radioDataSystem::emergencyWarning(){
	return radioData.emergencyWarning;
}

/// \brief
/// Clear Screen Request
/// \details
/// This function returns true if a Clear Screen Request has been send by the station. This way, some Radio Stations
/// try to display scrolling text. Boolean is rest to false after this function has been called. Until this function is called
/// , the boolean remains true.
bool radioDataSystem::clearScreen(){
	if(radioData.clearScreenRequest){
		radioData.clearScreenRequest = false;
		return true;
	} else {
		return false;
	}
}

/// \brief
/// Get Country Code
/// \details
/// This function returns the number of the current country. This way, one can determine the country he is currently in.
/// Used in cars.
unsigned int radioDataSystem::getCountryCode(){
	return ((radioData.blockA & 0xF000) >> 12);
}

/// \brief
/// Get Program Area
/// \details
/// This function returns the Program Area; according to International Standard.
unsigned int radioDataSystem::getProgramArea(){
	return ((radioData.blockA & 0x0F00) >> 8);
}

/// \brief
/// Get Program Refrence Number
/// \details
/// This function returns the Program Refrence Number; according to International Standard distributed by local authorities.
/// This way, one can determine the station.
unsigned int radioDataSystem::getProgramRefrence(){
	return (radioData.blockA & 0x00FF);
}

/// \brief
/// Get Message Group Type
/// \details
/// This function returns the Message Version / Group Type; A or B.
char radioDataSystem::getMessageGroupType(){
	if((radioData.blockB >> 11) & 1){
		return 'A';
	} else {
		return 'B';
	} 
}

/// \brief
/// Get Program Type
/// \details
/// This function returns the Program Type; according to International Standard.
/// List defines what kind of station this is; Popmusic, News, Podcast, Sport, etc.
unsigned int radioDataSystem::getProgramType(){
	return ((radioData.blockB & 0x3E0) >> 5);
}

/// \brief
/// Currently Music
/// \details
/// This function returns true if there is currently music, false in case of speech. Supported 
/// by some stations.
bool radioDataSystem::currentMusic(){
	return ((radioData.blockB >> 4) & 1);
}

/// \brief
/// Get Station Name
/// \details
/// This function returns the Station Name but doesn't update it. Thus, the user will have to call update()
/// at least once or call getStationName().
char* radioDataSystem::stationName(){
	return &radioData.receivedStationName[0];
}

/// \brief
/// Get Station Text
/// \details
/// This function returns the Station Text but doesn't updat it. Thus, the user will have to call update()
/// at least 10 times in a row (Station Text is not broadcast every second according to International Standards).
char* radioDataSystem::stationText(){
	return radioData.rdsText;
}

/// \brief
/// Get Traffic Program
/// \details
/// This function returns true if Traffic Announcements occur on this Station, false otherwise.
bool radioDataSystem::trafficProgram(){
	return ((radioData.blockB >> 10) & 1);
}

/// \brief
/// Get Traffic Announcement
/// \details
/// This function returns true if there currently is a Traffic Announcement.
bool radioDataSystem::trafficAnnouncement(){
	return ((radioData.blockB >> 5) & 1);
}

/// \brief
/// Get Hours
/// \details
/// This function returns the time in hours when tuned to a station which broadcasts RDS-A. Time info
/// is send at least every minute according to the International Standard. If no Time Info is received,
/// the time will be 0:0.
unsigned int radioDataSystem::hours(){
	return radioData.hours;
}

/// \brief
/// Get Minutes
/// \details
/// This function returns the time in minutes when tuned to a station which broadcasts RDS-A. Time info
/// is send at least every minute according to the International Standard. If no Time Info is received,
/// the time will be 0:0.
unsigned int radioDataSystem::minutes(){
	return radioData.minutes;
}

								//Radio Data System
//<<<---------------------------------------------------------------------------------------------->>>
					//Radio Data System Data and Program Item Number

/// \brief
/// Constructor
/// \details
/// This constructor has no mandatory parameters; they default to 0, 0 and 0.
programItemNumber::programItemNumber(unsigned int day, unsigned int hours, unsigned int minutes): 
	day(day), 
	hours(hours), 
	minutes(minutes)
{}

/// \brief
/// Set Data
/// \details
/// This function is used to set the Day, Hours and Minutes to the given (passed) values.
void programItemNumber::setData(unsigned int day, unsigned int hours, unsigned int minutes){
	day = day;
	hours = hours;
	minutes = minutes;
}

/// \brief
/// Constructor
/// \details
/// This constructor has no mandatory parameters and initializes a programItemNumber (composition).
radioDataSystemData::radioDataSystemData():
	PIN(programItemNumber()) 
{}
