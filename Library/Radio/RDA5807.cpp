/// @file

#include "hwlib.hpp"
#include "RDA5807.hpp"

/// \brief
/// Constructor
/// \details
/// This constructor has one mandatory parameter; the I2C bus. The Sequential Address defaults to 0x10,
/// the Index Address to 0x11 and the first read register defaults to 0x0A.
/// Leave the bandLimit empry or 0 for European / United States band Limits.
/// Pass 1 for Japanese, 2 for World Wide or 3 for East European Band Limits. A Radio Data System object
/// is also created (composition).
RDA5807::RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, const uint8_t address, const uint8_t firstReadRegister, const int bandLimit):
	Radio(bus, address, bandLimit),
	indexAddress(address + 1),					//0x10 for sequential access and 0x11 for indexed access
	firstReadRegister(firstReadRegister),
	radioData(radioDataSystem(bus))
{}

/// \brief
/// Send All Data
/// \details
/// This function is used to send the data array from index 2 to 8 (6 bytes). The first two bytes in the data
/// array are never send because they contain the Chip ID and reserved settings. They are here for completeness
/// and to prevent confusion.
void RDA5807::setData(){
	auto transaction = bus.write(address);
	for(unsigned int i = 2; i < 8; i++){
		transaction.write(data[i]);
	}
	hwlib::wait_ms(30);
}

/// \brief
/// Send One Byte
/// \details
/// This function is used to send one element of the data array. The index can be selected by the mandatory
/// parameter. The data first has to be manually set before being send to the chip (via the Index Addres 0x11).
void RDA5807::setData(const unsigned int regNumber){
	auto transaction = bus.write(indexAddress);
	transaction.write(regNumber);
	transaction.write((data[regNumber] & 0xFF00) >> 8);
	transaction.write(data[regNumber] & 0x00FF);
	hwlib::wait_ms(30);
}

/// \brief
/// Update and Send One Register
/// \details
/// This function is used to update and send the content of one element of the data array. It does the same
/// as setData(const unsinged int regNumber) but the data does not have to be set manually before sending the data.
/// The new value of the register, combined with the number of the register to update, have to be passed.
/// Then the element of the data array will be updated with the newly send data.
void RDA5807::setRegister(const unsigned int regNumber, const uint16_t value){
	data[regNumber] = value;
	auto transaction = bus.write(indexAddress);
	transaction.write(regNumber);
	transaction.write(((value & 0xFF00) >> 8));
	transaction.write((value & 0x00FF));
	hwlib::wait_ms(30);
}

/// \brief
/// Get All Data
/// \details
/// This function is used to receive the data in one register and place the received data in the status array
/// at the given index. The number from the register to get data from is the mandatory parameter.
void RDA5807::getStatus(const unsigned int regNumber){		//Addressing starts at 0x0A
	bus.write(indexAddress).write(firstReadRegister + regNumber);
	auto transaction = bus.read(indexAddress);
	status[regNumber] = transaction.read_byte() << 8;
	status[regNumber] |= transaction.read_byte();
	hwlib::wait_ms(30);
}

/// \brief
/// Get One Byte
/// \details
/// This function is used to receive all data, which is then placed in the status array for other functions
/// to use. Old data in the status array will be overwritten.
void RDA5807::getStatus(){
	bus.write(indexAddress).write(firstReadRegister);
	auto transaction = bus.read(indexAddress);
	for(unsigned int i = 0; i < 6; i++){
		status[i] = transaction.read_byte() << 8;
		status[i] |= transaction.read_byte();
	}
	hwlib::wait_ms(30);
}

/// \brief
/// Initialize Chip
/// \details
/// This function is used to initialize the chip and alter the settings so the chip can be used. It has one
/// optional parameter; wheter to start muted or not. It defaults to false, so when a setFrequency() or seekChannel()
/// function is called, music begins to play.
void RDA5807::begin(const bool muted){
	hwlib::wait_ms(1000);
	setMute(muted);
	normalAudio(true);
	setTune(true);
	hwlib::wait_ms(50);
	enableRadioData(true);
	powerUpEnable(true);
}

/// \brief
/// Choose Clock Supply
/// \details
/// This function is used to change the Clock Supply. When 0, the internal clock will be used. Otherwise the clock
/// received at RCLK will be used. When clockSource is set to 1, the external clock will be used and the directinput
/// bit will have to be set. The datasheet is unclear about other situations.
void RDA5807::setClockSupply(const bool clockSource, const bool directInput){
	if(clockSource){
		data[2] |= (1UL << 11);
	} else {
		data[2] &= ~(1UL << 11);
	}
	if(directInput){
		data[2] |= (1UL << 10);
	} else {
		data[2] &= ~(1UL << 10);
	}
	setData(2);
}

/// \brief
/// Set Internal Clock Frequency
/// \details
/// This function is used to set the Internal Clock Frequency. Leave empty for the default of 32.768kHz
/// There generally is no reason to differ from the default. Other options are 12, 13, 19, 24, 26 or 38Mhz.
void RDA5807::setClockFrequency(const unsigned int frequency){
	data[2] &= ~(7UL << 4); 	//Clear all bits first (also equal to 32.768kHz)
	switch(frequency){
		case 12:
			data[2] |= (1UL << 4);
			break;
		case 13:
			data[2] |= (2UL << 4);
			break;
		case 19:
			data[2] |= (3UL << 4);
			break;
		case 24:
			data[2] |= (5UL << 4);
			break;
		case 26:
			data[2] |= (6UL << 4);
			break;
		case 38:
			data[2] |= (7UL << 4);
	}
	setData(2);
}

/// \brief
/// Get Internal CLock Frequency
/// \details
/// This function is used to get the Internal Clock Frequency. All returned values are in MHz, except
/// 32768; that one is in kHz.
unsigned int RDA5807::getClockFrequency(){
	const auto clockFrequency = (data[2] & 0x0070) >> 4;
	switch(clockFrequency){
		case 1:
			return 12;
		case 2:
			return 13;
		//Yes, there is no 4
		case 3:
			return 19;
		case 5:
			return 24;
		case 6:
			return 26;
		case 7:
			return 38;
		default:
			return 32;
	}
}

/// \brief
/// Change Demodulate Method
/// \details
/// This function is used to change the Demodulate Method. According to the datasheet the new method
/// could increase the Signal Strength by approximately 1dB.
void RDA5807::demodulateMethod(const bool newMethod){
	if(newMethod){
		data[2] |= (1UL << 2);
	} else {
		data[2] &= ~(1UL << 2);
	}
	setData(2);
}

/// \brief
/// Get Demodulate Method
/// \details
/// This function returns true if the new Demodulate Method is used, or false when the usual method is used.
bool RDA5807::newDemodulate(){
	return (data[2] >> 2) & 1;
}

/// \brief
/// Reset
/// \details
/// This function resets the chip through the built-in soft-reset function. After restting, the chip
/// will be initialized again.
void RDA5807::reset(){
	data[2] |= (1UL << 1);
	setData(2);
	hwlib::wait_ms(50);
	begin();
}

/// \brief
/// Power Up
/// \details
/// This function is used to initialize the chips internals. Not for use; see standBy().
void RDA5807::powerUpEnable(const bool enable){
	if(enable){
		data[2] |= 1UL;
	} else {
		data[2] &= ~1UL;
	}
	setData(2);
}

/// \brief
/// Get Signal Strength
/// \details
/// This function returns the current Signal Strength. It can vary from 0 to 63 where lower means
/// a better Signal Strength.
unsigned int RDA5807::signalStrength(){
	status[1] &= ~0xFC00;
	getStatus(1);
	return ((status[1] & 0xFC00) >> 9);
}

/// \brief
/// Mute / Unmute
/// \details
/// This function is used to mute or unmute the chip. This function takes one mandatory parameter.
/// When true, the chip will mute. It will unmute otherwise.
void RDA5807::setMute(const bool mute){
	if(mute){
		data[2] &= ~(1UL << 14);
	} else {
		data[2] |= (1UL << 14);
	}
	setData(2);
}

/// \brief
/// Is Muted
/// \details
/// This function returns true if the chip is muted, false if unmuted.
bool RDA5807::isMuted(const char side){
	return !(data[2] >> 14) & 1;
}

/// \brief
/// Low / Normal Impedance Output
/// \details
/// This function is used to set wether the output has a high impedance or a normal impedance. Generally,
/// a normal impedance is the way to go.
void RDA5807::normalAudio(const bool normal){
	if(normal){
		data[2] |= (1UL << 15);
	} else {
		data[2] &= ~(1UL << 15);
	}
	setData(2);
}

/// \brief
/// Get Normal Audio
/// \details
/// This function returns true if the outputed signal is suitable for normal impedance or false when 
/// it is aimed at high impedance.
bool RDA5807::isNormalAudio(){
	return (data[2] >> 15) & 1;
}

//Done
void RDA5807::setBassBoost(const bool boost){
	if(boost){
		data[2] |= (1UL << 12);
	} else {
		data[2] &= ~(1UL << 12);
	}
	setData(2);
}

bool RDA5807::bassBoosted(){
	return (data[2] >> 12) & 1;
}

bool RDA5807::stereoReception(){
	getStatus(0);
	return (status[0] >> 10) & 1;
}

//Done
void RDA5807::setStereo(const bool stereo){
	if(stereo){
		data[2] &= ~(1UL << 13);
	} else {
		data[2] |= (1UL << 13);
	}
	setData(2);
}

bool RDA5807::isStereo(){
	return !(data[2] >> 13) & 1;		//Stereo is 0, mono is 1
}

//Done
void RDA5807::setSpacing(const unsigned int spacing){
	data[3] &= ~3UL;
	if(spacing == 100000){
		data[3] |= 3UL;
	} else if (spacing == 50000){
		data[3] |= (1UL << 1);
		data[3] &= ~1UL;
	} else if (spacing == 20000){
		data[3] |= 1UL;
		data[3] &= ~(1UL << 1);
	} else {
		data[3] &= ~3UL;
	}
	setData(3);
}

unsigned int RDA5807::getSpacing(){
	const auto spacing = (data[3] & 0x0003);
	if(spacing == 0){
		return 100000;
	} else if (spacing == 1){
		return 200000;
	} else if (spacing == 2){
		return 50000;
	} else {
		return 25000;
	}
}

bool RDA5807::setFrequency(const float frequency, const bool autoTune){
	radioData.reset();		//Clear Received RDS-Data since this is not useful anymore and will only slow the process down
	if(autoTune){
		data[3] |= (1UL << 4);
	} else {
		data[3] &= ~(1UL << 4);
	}
	data[3] &= ~(0x1FF << 6); //Unset bits representing frequency.
	auto tunableFrequency = 1000;
	//Prevent tuning to illegal Frequencies
	if((bandLimit == 3 && frequency >= 65 && frequency <= 76) || (bandLimit == 2 && frequency >= 76 && frequency <= 108) || (bandLimit == 1 && frequency >= 76 && frequency <= 91) || (bandLimit == 0 && frequency >= 87 && frequency <= 108)){
		if(bandLimit == 3){
			tunableFrequency = frequency * 10 - 650;
		} else if(bandLimit == 1 || bandLimit == 2){
			tunableFrequency = frequency * 10 - 760;
		} else {
			tunableFrequency = frequency * 10 - 870;
		}
	} else {
		//If outside legal range; tune to first legal frequency.
		switch(bandLimit){
			case 3:		//Eastern Europe
				tunableFrequency = 65 * 10 - 650;
				break;
			case 0:		//World-Wide
				tunableFrequency = 87 * 10 - 870;
				break;
			default:
				tunableFrequency = 76 * 10 - 760;
		}
	}
	if((data[3] >> 2) & 3){
		tunableFrequency = frequency * 10 - 650;
	} else if((data[3] >> 2) & 1 || (data[3] >> 2) & 2){
		tunableFrequency = frequency * 10 - 760;
	} else {
		tunableFrequency = frequency * 10 - 870;
	}
	data[3] |= (tunableFrequency << 6);
	setData(3);
	hwlib::wait_ms(100);
	//Find out if tune action was completed.
	getStatus(0);
	return (status[0] >> 14) & 1;
	//STC bit is set high when tuning completes or low when it fails. Tune bit is automatically set low when tuning completes.
}

void RDA5807::setFrequency(const float frequency){
	setFrequency(frequency, true);
}

float RDA5807::getFrequency(){
	getStatus(0);
	auto receivedFrequency = (status[0] & 0x3FF); //Only keep last 10 bits
	if(bandLimit == 3){
		return (65.0 + (receivedFrequency * 0.025));
	} else if(bandLimit == 2){
		return (76.0 + (receivedFrequency * 0.05));
	} else if (bandLimit == 1){
		return (76.0 + (receivedFrequency * 0.02));
	} else {
		return (87.0 + (receivedFrequency * 0.1));
	}
}

unsigned int RDA5807::getIntFrequency(){
	return (getFrequency() * 10);
}

bool RDA5807::isStation(){
	getStatus(1);
	return (status[1] & 0x4100);
}

bool RDA5807::isReady(){
	getStatus(1);
	return (status[1] & 0x4080);
}

bool RDA5807::isTuned(){
	getStatus(0);
	return (status[0] & 0x4000);
}

void RDA5807::setBandLimit(const unsigned int limit){
	data[3] &= ~(3UL << 2);			//Unset bits representing US/European Band Limit.
	bandLimit = limit;
	if(limit >= 0 && limit <= 3){	//Else it becomes/stays 0
		data[3] |= (limit << 2);
	}
	setData(3);
}

unsigned int RDA5807::hasBandLimit(){
	return bandLimit;
}

void RDA5807::setVolume(const uint8_t volume){
	data[5] &= ~0x000F;
	data[5] |= (volume & 0x000F);
	setData(5);
}

unsigned int RDA5807::getVolume(){
	return (data[5] & 0x000F);
}

void RDA5807::setTune(const bool tune){
	if(tune){
		data[3] |= (1UL << 4);
	} else {
		data[3] &= ~(1UL << 4);
	}
	setData(3);
}

void RDA5807::standBy(const bool sleep){
	if(sleep){
		data[2] &= ~1UL;
	} else {
		data[2] |= 1UL;
	}
	setData(2);
}

bool RDA5807::isStandBy(){
	return !(data[2] & 1);
}

//Done
void RDA5807::seekChannel(const unsigned int direction, const bool wrapContinue){
	radioData.reset();
	data[2] |= (1UL << 8);		//Set seek mode
	if(direction > 0){
		data[2] |= (1UL << 9);	//Seek up
	} else {
		data[2] &= ~(1UL << 9);	//Seek down
	}
	if(wrapContinue){
		data[2] &= ~(1UL << 7);
	} else {
		data[2] |= (1UL << 7);
	}
	setData(2);
}

void RDA5807::setSeekThreshold(const uint8_t threshold){
	data[5] &= ~0xF00;
	data[5] |= (threshold & 0xF00);
	setData(5);
}

bool RDA5807::seekCompleted(){
	getStatus(0);
	return (status[0] >> 14) & 1;
}

void RDA5807::seek(const unsigned int direction){
	seekChannel(direction);
}

void RDA5807::enableRadioData(const bool enable){
	if(enable){
		data[2] |= (1UL << 3);
	} else {
		data[2] &= ~(1UL << 3);
	}
	setData(2);
}

bool RDA5807::radioDataEnabled(){
	return (data[2] >> 3) & 1;
}

bool RDA5807::radioDataReady(){
	getStatus(0);
	return (status[0] >> 15) & 1;
}

bool RDA5807::radioDataSynced(){
	getStatus(0);
	return (status[0] >> 12) & 1;
}

unsigned int RDA5807::radioDataErrors(const unsigned int block){
	getStatus(1);
	if(block == 1){
		return (status[1] & 0x000C);
	} else {
		return (status[1] & 0x0003);
	}
}

void RDA5807::printRawRadioData(){
	radioData.rawData();
}

char* RDA5807::stationName(){
	return radioData.stationName();
}

char* RDA5807::getStationText(){
	return radioData.getStationText();
}

char* RDA5807::stationText(){
	return radioData.stationText();
}

void RDA5807::updateRadioData(){
	radioData.update();
}
