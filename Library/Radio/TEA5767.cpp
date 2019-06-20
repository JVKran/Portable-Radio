/// @file

#include "hwlib.hpp"
#include "TEA5767.hpp"

float floor(float input){
    if(input < 0){
        return float(int(input - 1));   
    } else {
        return float(int(input));
    }
}

/// \brief
/// Constuctor
/// \details
/// This constructor has one mandatory parameter; the I2C bus. The address defaults
/// to 0x60. Leave the bandLimit empty or 0 for no bandlimits (EU/US) or 1 for use in
/// Japan. The module is automatically muted after initialisation and best settings are set.
TEA5767::TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, int bandLimit, uint8_t address): Radio(bus, bandLimit, address){
	setMute(true);
	audioSettings();
}

/// \brief
/// Send Data
/// \details
/// This function is used to send the data array, which is updated and changed in other functions,
/// to the TEA5767 chip.
void TEA5767::setData(){
	bus.write(address).write(data, 5);
	hwlib::wait_ms(30);		//Necessary to prevent strange things from happening
}

/// \brief
/// Get Data
/// \details
/// This function is used to receive data, which is placed in the status array and 
/// interpreted by other functions, from the TEA5767 chip.
void TEA5767::getStatus(){
	bus.read(address).read(status, 5);
	hwlib::wait_ms(30);		//Necessary to prevent strange things from happening
}


/// \brief
/// Change Band Limit
/// \details
/// This function is used to change the Band Limit. Leave empty or 1 to set bandLimit or 0 for
/// no Band Limit. If the Band Limit is set, the chip will only be able to tune 76 MHz to 91 MHz.
/// If the Band Limit is not set, the entire range from 87.5 MHz to 108 MHz will be available to 
/// tune to.
void TEA5767::setBandLimit(const unsigned int limit){
	bandLimit = limit;
	if(limit > 0){
		data[3] |= (1UL << 5);
	} else {
		data[3] &= ~(1UL << 5);
	}
	setData();
}

/// \brief
/// Change Clock Frequency
/// \details
/// This function is used to change the Clock Frequeny. The parameter can be 6, 13 or 32. However,
/// in almost all cases there is no reason to change the standard value 32 (32768kHz). It is also possible
/// to use an external clock which can be connected to XTAL2. This library only support 32768kHz tuning.
void TEA5767::setClockFrequency(const unsigned int frequency){
	if (frequency == 13){
		data[3] &= ~(1UL << 4);		//XTAL
		data[4] &= ~(1UL << 7);		//PLLref
	} else if (frequency == 6){
		data[3] &= ~(1UL << 4);
		data[4] |= (1UL << 7);	
	} else {
		data[3] |= (1UL << 4);
		data[4] &= ~(1UL << 7);
	}
	setData();
}

/// \brief
/// Calculate And Set PLL
/// \details
/// This function handles the calculation of the PLL word. PLL is the system used for tuning. In
/// combination with the PLLref and XTAL bits, this is the technique to tune the chip with. It can be
/// calculated in two ways for two different approaches; High or Low Side Injection. The result
/// of this function is that the PLL word has been stored in the buffer; the data array.
void TEA5767::setPLL(const float frequency, unsigned int hilo){
	unsigned int pllFrequency;
	if(hilo == 1){
		pllFrequency = (4 * (frequency * 1000000 + 225000) / 32768);
		data[2] |= (1UL << 4);
	} else {
		pllFrequency = (4 * (frequency * 1000000 - 225000) / 32768);
		data[2] &= ~(1UL << 4);
	}
	data[0] = pllFrequency >> 8;
	data[1] = pllFrequency & 0xFF;
	//No update to registers for flexibility.
}

/// \brief
/// High or Low Side Injection
/// \details
/// This function determines which kind of Injection results in the best audio. 
/// We speak of High Side Injection if the Frequency of the Local Osscilator is higher than the
/// tuned frequency and Low Side Injection if the Frequency of the Local Osscilator is lower than
/// the tuned frequency. Determining which one of the two possibilities is better is 
/// done by tuning to the frequency with High Side Injection, measuring the Signal Strength,
/// tuning to the frequeny with Low Side Injection and then comparing the Singnal Strength.
/// The best kind of injection is returned; low (0), or high (1).
int TEA5767::testHiLo(const float frequency){
	setPLL(frequency, 1);
	setData();
	hwlib::wait_ms(30);
	int highStrentgh = signalStrength();
	setPLL(frequency, 0);
	setData();
	hwlib::wait_ms(30);
	int lowStrentgh = signalStrength();
	if (highStrentgh >= lowStrentgh){
		return 1;
	} else {
		return 0;
	}
}

/// \brief
/// Set Frequency
/// \details
/// This function tunes to the given frequency. If no frequency is provided, the chip will tune to the first
/// legal frequency The user can also choose to force the tuning with High or Low Side Injection. If nothing is passed,
/// testHiLo() will be called to determine the best option. During the tuning, the audio is muted
/// to prevent loud pops.
void TEA5767::setFrequency(const float frequency, const int hiLoForce){
	if((((bandLimit && frequency <= 91) || (bandLimit && frequency >= 76) || ((!bandLimit && frequency <= 108) || (!bandLimit && frequency >= 87.5))) && frequency != -1)){
		setMute(true);
		if(hiLoForce == -1){
			setPLL(frequency, testHiLo(frequency));
		} else if(hiLoForce == 0 || hiLoForce == 1){
			setPLL(frequency, hiLoForce);
		}
		setMute(false);
	} else {
		if(bandLimit){
			setFrequency(76);
		} else {
			setFrequency(87.5);
		}
	}
	setData();
}

/// \brief
/// Get Frequency
/// \details
/// This function returns the currently tuned frequency of the chip. Wheter it is using
/// High or Low Side Injection can only be determined from the data array; not from the status.
/// However, this is not a problem; the search mode uses the data array to tune to frequencies too.
float TEA5767::getFrequency(){
	getStatus();
	int pllFrequency = ((status[0]&0x3F) << 8) + status[1];
	if((data[2] >> 4) & 1){//If High side injection is set
		return (((pllFrequency / 4.0) * 32768.0) - 225000.0) / 1000000.0;
	} else {
		return (((pllFrequency / 4.0) * 32768.0) + 225000.0) / 1000000.0;
	}
}

/// \brief
/// Mute Unmute L and R
/// \details
/// This function takes one parameter that determines if the audio has to be muted or unmuted.
void TEA5767::setMute(const bool mute){
	if(mute){
		data[0] |= (1UL << 7);
		data[2] |= (3UL << 1);
	} else {
		data[0] &= ~(1UL << 7);
		data[2] &= ~(3UL << 1);
	}
	setData();
}

/// \brief
/// Mute Unmute L or R
/// \details
/// This function takes two parameters which define what side has to be muted or unmuted. If L or R
/// is muted, the audio signal will be mono. Otherwise it will be automatically set to stereo.
void TEA5767::setMute(const char side, const bool mute){
	if(side == 'l'){
		if(mute){
			data[2] |= (1UL << 1);
		} else {
			data[2] &= ~(1UL << 1);
		}
	} else if(side == 'r'){
		if(mute){
			data[2] |= (1UL << 2);
		} else {
			data[2] &= ~(1UL << 2);
		}
	}
	if((data[2] >> 1) & 1 && (data[2] >> 2) & 1){
		setStereo(true);
	} else {
		setStereo(false);
	}
}

/// \brief
/// Stand-By
/// \details
/// This function takes one parameter that determines if the chip has to wake-up or go to Stand-By mode.
/// While Stand-By the chip can still communicate, though it only draws 26uA in comparison when it is in use (26mA).
void TEA5767::standBy(const bool sleep){
	if(sleep){
		data[3] |= (1UL << 6);
		setMute(true);
	} else {
		data[3] &= ~(1UL << 6);
		setMute(false);
	}
}

/// \brief
/// Get Signal Strength
/// \details
/// This function returns the currently Signal Strength of the current tuned Frequency. It has to overwrite the
/// values for them to update. The Signal Strength is the only value for which this is needed.
unsigned int TEA5767::signalStrength(){
	setData(); //According to datasheet: Overwrite values to update
	getStatus();
	return status[3];
}

/// \brief
/// Set Mono/Stereo
/// \details
/// This function takes one parameter that determines if the chip has to output Mono or Stereo sound.
void TEA5767::setStereo(const bool stereo){
	if (stereo){
		data[2] &= ~(1UL << 3);
	} else {
		data[2] |= (1UL << 3);
	}
	setData();
}

/// \brief
/// Stereo/Mono Reception
/// \details
/// This function returns true if the currenty tuned frequency supplies a stereo signal, or false when the 
/// currently tuned frequency supplies a mono signal.
bool TEA5767::stereoReception(){
	getStatus();
	return (status[2] >> 7) & 1;
}

/// \brief
/// Audio Quality Settings
/// \details
/// This function takes three boolean parameters; Stereo Noise Cancelling (switches to mono in case of a weak signal),
/// High Cut Control (reduces high frequencies) and Soft Mute (reduces crisps in the signal) where true means they should
/// be turned on.
void TEA5767::audioSettings(const bool SNC, const bool HCC, const bool SM){
	if (SNC){
		data[3] |= (1UL << 1);
	} else {
		data[3] &= ~(1UL << 1);
	}
	if (HCC){
		data[3] |= (1UL << 2);
	} else {
		data[3] &= ~(1UL << 2);
	}
	if (SM){
		data[3] |= (1UL << 3);
	} else {
		data[3] &= ~(1UL << 3);
	}
}

/// \brief
/// Set Programmable Ports
/// \details
/// This function takes two mandatory parameters; two booleans representing the state of two Software Programmable Ports.
/// Software Programmable Port 1 can also be used as Search Indicator. Therefore you have to set searchIndicator to true.
void TEA5767::setPort(const bool portOne, const bool portTwo, const bool searchIndicator){
	if(portOne && !searchIndicator){
		data[2] |= 1UL;
	} else {
		data[2] &= ~1UL;
	}
	if(searchIndicator){
		data[3] |= 1UL;
	} else {
		data[3] &= ~1UL;
	}
	if(portTwo){
		data[3] |= (1UL << 7);
	} else {
		data[3] &= ~(1UL << 7);
	}
}

/// \brief
/// Search Stations Loop
/// \details
/// This function takes two parameters; the direction to search in (1:up, 0:down) and the Quality Threshold. The 
/// Quality Threshold determines how good the Signal Strength has to be for the chip to really tune to it (0-5: equal to signal-
/// strength of 0-200. 3 is a good average, 4 will skip most, 2 will give false positives). This function uses the Built-In Auto Search Mode of the 
/// TEA5767. This makes it automatically add 100kHz to the frequency until it has found one with a good enough quality. It will then raise the RF
/// (Ready Flag).
/// If a Band Limit is reached the BLF (Band Limit Flag) will be raised. This function will then set the frequency back to the lowest
/// possible one (in case of search up) or highest one (in case of search down) so a circulair search is performed until the loop is complete.
/// If after one band loop no valid Frequency has been found, the loop will exit and the chip will tune back to the Start Frequency.
/// It can occur that one Frequency gets found more than once in one single search or a specific Frequency is found in one search, but not in another.
/// This can occur because this function asks for realtime info every loop and thus, requires precise timing; You Absolutely should not change any
/// timings. If you decide to do so be advised this takes large amounts of tuning.
/// Somehow, this function is very unreliable with HWLIB, but not with Wire.h.

void TEA5767::searchLoop(const unsigned int direction, const unsigned int qualityThreshold){
	setMute(true);
	float startFrequency = getFrequency();
	if(direction == 1){
		setPLL(getFrequency() + 0.098304, 1);
		data[2] |= (1UL << 7);
	} else if (direction == 0){
		setPLL(getFrequency() - 0.098304, 1);
		data[2] &= ~(1UL << 7);
	}
	//set HiLo and Frequency
	data[2] |= (1UL << 4);
	//Search Mode On
	data[0] |= (1UL << 6);
	//SSL Highest Quality
	data[2] |= (qualityThreshold << 5);
	//Unset search indicator
	data[3] &= ~(1);
	//Set XTAL to 1 and PLLREF to 0 for 32768kHz Clock Frequency
	data[3] |= (1UL << 4);
	data[4] &= ~(1UL << 7);
	setData();
	hwlib::wait_ms(50);
	float foundFrequency = 0.0;
	unsigned int totalBandFrequencies;
	unsigned int loops = 0;
	if(bandLimit){
		totalBandFrequencies = 170;				//Little more to be sure every frequency has been searched.
	} else {
		totalBandFrequencies = 215;	
	}
	while(loops < totalBandFrequencies){
		//If Station has been Found, BandLimit has not been reached and another frequency is found; tune to that.
		if((status[0] >> 7) & 1 && (status[0] >> 6) & 0){
			foundFrequency = getFrequency();		//Calling funciton isn't fast enough
			//Unset Search Mode
			data[0] &= ~(1UL << 6);
			break;
		} else if ((status[0] >> 6) & 1){		//If bandlimit has been reached
			if(direction == 1){
				if(bandLimit){
					setPLL(76, 1);
				} else {
					setPLL(88, 1);
				}
			} else {
				if(bandLimit){
					setPLL(91, 1);
				} else {
					setPLL(108, 1);
				}
			}
			setData();
		}
		bus.read(address).read(status, 5);
		if(data[0] > 0x2D){
			// Allmost all stations under 96MHz get found with this delay
			hwlib::wait_ns(30000);
		} else {
			// Allmost all stations above 96MHz get found with this delay
			hwlib::wait_ns(50000);
		}
		loops++;
	}
	foundFrequency = getFrequency();
	if(foundFrequency != startFrequency){
		setFrequency(foundFrequency);
	} else {
		setFrequency(startFrequency);
	}
	setMute(false);
}

/// \brief
/// Search Stations Once
/// \details
/// This function takes two parameters; the direction to search in (1:up, 0:down) and the Quality Threshold. The 
/// Quality Threshold determines how good the Signal Strength has to be for the chip to really tune to it (0-5: equal to signal-
/// strength of 0-200. 3 is a good average, 4 will skip most, 2 will give false positives). This function uses the Built-In Auto Search Mode of the 
/// TEA5767. This makes it automatically add 100kHz to the frequency until it has found one with a good enough quality. It will then raise the RF
/// (Ready Flag).
/// If a Band Limit is reached the BLF (Band Limit Flag) will be raised. This function will then set the frequency back to the lowest
/// possible one (in case of search up) or highest one (in case of search down) to prevent tuning out of range. This function only searches once
/// without communicating with the TEA5767. After 200ms the chip is asked wether it found a station, reached a band limit or has not found a station.
/// If a station is found, the chip will tune to that frequency. Otherwise it will remain tuned to the start frequency.
void TEA5767::singleSearch(const unsigned int direction, const unsigned int qualityThreshold){
	setMute(true);
	float startFrequency = getFrequency();
	if(direction == 1){
		setPLL(getFrequency() + 0.098304, 1);
		data[2] |= (1UL << 7);
	} else if (direction == 0){
		setPLL(getFrequency() - 0.098304, 1);
		data[2] &= ~(1UL << 7);
	}
	//set HiLo and Frequency
	data[2] |= (1UL << 4);
	//Search Mode On
	data[0] |= (1UL << 6);
	//SSL Highest Quality
	data[2] |= (qualityThreshold << 5);
	//Unset search indicator
	data[3] &= ~(1);
	//Set XTAL to 1 and PLLREF to 0 for 32768kHz Clock Frequency
	data[3] |= (1UL << 4);
	data[4] &= ~(1UL << 7);
	setData();
	hwlib::wait_ms(200);
	float foundFrequency = 0.0;
	getStatus();
	//If Station has been Found, BandLimit has not been reached and another frequency is found; tune to that.
	if((status[0] >> 7) & 1 && (status[0] >> 6) & 0){
		foundFrequency = getFrequency();		//Calling funciton isn't fast enough
		//Unset Search Mode
		data[0] &= ~(1UL << 6);
	} else if ((status[0] >> 6) & 1){		//If bandlimit has been reached
		if(direction == 1){
			if(bandLimit){
				setPLL(76, 1);
			} else {
				setPLL(88, 1);
			}
		} else {
			if(bandLimit){
				setPLL(91, 1);
			} else {
				setPLL(108, 1);
			}
		}
		setData();
	} else {
		foundFrequency = startFrequency;
	}
	foundFrequency = getFrequency();
	float tunableFrequency = foundFrequency;
	setFrequency(tunableFrequency);
	setMute(false);
}

/// \brief
/// Alternative Station Search
/// \details
/// This function largely does the same as searchLoop(). However, some chips are partially faulty resulting in the inability to auto search. This
/// function enables those chips to search as well. Despite faulty chips, this function is also usefull for users who want more control over their radio. 
/// The quality threshold is more effictive and a larger accuracy can be achieved. The user can also select what distance has to be in between searches to
/// prevent stations from getting found twice; thus the user can chose if he wants a bigger chance of finding all stations or he wants to find stations only once.
/// The user has to let go of some performance though; its totally software-based.
/// The default value (0.4) is a nice average (most stations will get found without doubles). With 0.2 you should are able to find all stations (minimum FM-frequency spacing).
/// In short: This function always finds all stations and the best signal strengths; though it really is slow (2.5s).
/// This function takes two parameters; the direction to search in (1:up, 0:down) and the Quality Threshold. The 
/// Quality Threshold determines how good the Signal Strength has to be for the chip to really tune to it (0-5: equal to signal-
/// strength of 0-200. 3 is a good average, 4 will skip most, 2 will give false positives). This function partially uses the Built-In Auto Search Mode of the 
/// TEA5767. This requires the Microcontroller to keep incrementing or decrementing the Frequency. The TEA5767 will check 
/// if the Signal is clear enough; It will then raise the RF (Ready Flag).
/// If a Band Limit is reached the BLF (Band Limit Flag) will be raised. This function will then set the frequency back to the lowest
/// possible one (in case of search up) or highest one (in case of search down) so a circulair search is performed until the loop is complete.
/// If after one loop no valid Frequency has been found, the loop will exit and the chip will tune back to the Start Frequency.
/// It can occur that one Frequency gets found more than once. This is often the case near broadcasters. It can also happen a Frequency gets found
/// in one search, but not in another one (~1 in 8 chance).

void TEA5767::altSearch(const unsigned int direction, const unsigned int inQualityThreshold, const float distance){
	setMute(true);
	unsigned int qualityThreshold;
	if(inQualityThreshold > 5){
		qualityThreshold = 5;
	} else {
		qualityThreshold = inQualityThreshold;
	}
	float startFrequency = getFrequency();
	//set HiLo
	data[2] |= (1UL << 4);
	if(direction == 1){
		setPLL(getFrequency() + 0.05, 1);
		data[2] |= (1UL << 7);
	} else {
		setPLL(getFrequency() - 0.05, 1);
		data[2] &= ~(1UL << 7);
	}
	//Search Mode On
	data[0] |= (1UL << 6);
	//SSL Highest Quality
	data[2] |= (3UL << 5); 		//3 has proven to be the only usable option
	//Mute Volume
	data[0] |= (1UL << 7);
	data[2] |= (3UL << 1);
	//Unset search indicator
	data[3] &= ~(1);
	//Set XTAL to 1 and PLLREF to 0 for 32768kHz Clock Frequency
	data[3] |= (1UL << 4);
	data[4] &= ~(1UL << 7);
	setData();
	float foundFrequency = startFrequency;
	hwlib::wait_ms(100);
	unsigned int totalBandFrequencies;
	unsigned int loops = 0;
	if(bandLimit){
		totalBandFrequencies = 170;				//Little more to be sure every frequency has been searched.
	} else {
		totalBandFrequencies = 215;	
	}
	while(loops < totalBandFrequencies){
		getStatus();
		//If Station has been Found and another frequency is found; tune to that.
		if((status[0] >> 7) & 1){
			data[2] |= (1UL << 4);
			foundFrequency = getFrequency();
			break;
		} else if ((status[0] >> 6) & 1 && (status[0] >> 6) & 0){		//If bandlimit has been reached
			if(direction == 1){
				if(bandLimit){
					setPLL(76, 1);
				} else {
					setPLL(88, 1);
				}
				setData();
			} else {
				if(bandLimit){
					setPLL(91, 1);
				} else {
					setPLL(108, 1);
				}
				setData();
			}
		} else {
			if(direction == 1){
				setPLL(getFrequency() + 0.1, 1);
				loops++;
			} else if (direction == 0){
				setPLL(getFrequency() - 0.1, 1);
				loops++;
			}
			setData();
		}
	}
	//Unset Search Mode
	data[0] &= ~(1UL << 6);
	getStatus();
	float bestFrequency = 0.0;
	int currentStrength = 0;
	int bestStrength = 0;
	for(float i = foundFrequency - 0.5; i < foundFrequency + 0.5; i+=0.05){
		setPLL(i, 1);
		setData();
		hwlib::wait_ms(2);
		currentStrength = signalStrength();
		if(currentStrength > bestStrength){
			bestFrequency = i;
			bestStrength = currentStrength;
		}
	}
	setPLL(bestFrequency, testHiLo(bestFrequency));
	setData();
	if(signalStrength() < qualityThreshold * 40 || (direction == 1 && bestFrequency <= startFrequency - distance) || (direction == 0 && bestFrequency < startFrequency - distance)){
		//If the audio quality is not good enough; keep searching.
		altSearch(direction, qualityThreshold);
	} else {
		setMute(false);
	}
}

/// \brief
/// Search Stations Loop with Startfrequency
/// \details
/// This function does the same as searchLoop() but first tunes to a given startFrequency.
void TEA5767::searchLoop(const float startFrequency, const unsigned int direction, const unsigned int qualityThreshold){
	setPLL(startFrequency, 1);
	searchLoop(direction, qualityThreshold);
}

/// \brief
/// Search Stations Single with Startfrequency
/// \details
/// This function does the same as singleSearch() but first tunes to a given startFrequency.
void TEA5767::singleSearch(const float startFrequency, const unsigned int direction, const unsigned int qualityThreshold){
	setPLL(startFrequency, 1);
	singleSearch(direction, qualityThreshold);
}

/// \brief
/// Alternative Station Search with Startfrequency
/// \details
/// This function does the same as altSearch() but first tunes to a given startFrequency.
void TEA5767::altSearch(const float startFrequency, const unsigned int direction, const unsigned int qualityThreshold, const float distance){
	setPLL(startFrequency, 1);
	altSearch(direction, qualityThreshold, distance);
}

/// \brief
/// Get Port State
/// \details
/// This function returns the state of the given port; Software Programmable Port 1 or 2.
bool TEA5767::getPort(const unsigned int port){
	if(port == 1){
		return (data[2] & 1);
	} else {
		return ((data[3] >> 7) & 1);
	}
}

/// \brief
/// Get Search State
/// \details
/// This function returns true if the TEA5767 is in Search Mode.
bool TEA5767::inSearch(){
	return (data[0] >> 6) & 1;
}

/// \brief
/// Get Mute State
/// \details
/// This function returns true if the given speaker 'l', 'r' or 'a' (all) is muted.
bool TEA5767::isMuted(const char side){
	if(side == 'l'){
		return (data[2] >> 1) & 1;
	} else if (side == 'r'){
		return (data[2] >> 2) & 1;
	} else {
		return (data[2] >> 1) & 1 && (data[2] >> 2) & 1;
	}
}

/// \brief
/// Get High Side Injection
/// \details
/// This function returns true if the TEA5767 is tuned with High Side Injection.
bool TEA5767::highSide(){
	return (data[2] >> 4) & 1;
}

/// \brief
/// Get StandBy State
/// \details
/// This function returns true if the TEA5767 is muted.
bool TEA5767::isStandBy(){
	return (data[3] >> 6) & 1;
}

/// \brief
/// Get Clock Frequency
/// \details
/// This function returns the Clock Frequency of the TEA5767
unsigned int TEA5767::clockFrequency(){
	if ((data[3] >> 5) & 0 && (data[4] >> 7) & 0){
		return 13;
	} else if ((data[3] >> 5) & 1 && (data[4] >> 7) & 0){
		return 32;
	} else {
		return 6;
	}
}

/// \brief
/// Get Band Limit
/// \details
/// This function returns true if the TEA5767's FM-range is limited.
bool TEA5767::hasBandLimit(){
	return(data[0] >> 6) & 1;
}

