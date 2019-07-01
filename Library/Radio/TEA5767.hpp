/// @file

#ifndef __TEA5767_HPP
#define __TEA5767_HPP

#include "Radio.hpp"

/// \brief
/// TEA5767 Interface
/// \details
/// This is an interface that simplifies the use of TEA5767 chips. Tuning, Muting
/// and setting of Frequencies are some of its abilities. It is compatible with the module
/// and breakout, though the breakout has hardwareproblems. It doesn't support searching
/// for channels that well.
/// 
///	Some supported operations are:
///		- Set/Get Frequency
///		- Set/Get Stereo
///		- Set/Get Mute
///		- Set/Get Volume
///		- Automatic Search
///		- Set/Get Stand By
///		- Set/Get Software Programmable Ports
///
/// ~~~~~~~~~~~~~~~{.cpp}
/// auto scl = target::pin_oc( target::pins::d8 );
/// auto sda = target::pin_oc( target::pins::d9 );
/// auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
/// 
/// auto radio = TEA5767(i2c_bus);
/// radio.setStereo(true);
/// radio.audioSettings(true, true, true);
/// radio.setFrequency(100.7);
/// for(;;){
///     hwlib::cout << radio.signalStrength() << hwlib::endl;
///     hwlib::wait_ms(10000);
/// }
/// ~~~~~~~~~~~~~~~
class TEA5767 : public Radio {
	private:
		// I2C Communication
		uint8_t data[5] = {0x30, 0x1F, 0x10, 0x10, 0xC0};
		uint8_t status[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
		void setData() override;
		void getStatus() override;

		//Calucaltion of PLL word
		int testHiLo(const float frequency);
		void setPLL(const float frequency, unsigned int hilo);
	public:
		TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, int bandLimit = 0, uint8_t address = 0x60);

		//Basic Settings
		void setClockFrequency(const unsigned int frequency = 32) override;
		unsigned int getClockFrequency() override;

		void setBandLimit(const unsigned int limit = 1) override;
		unsigned int hasBandLimit() override;

		void standBy(const bool sleep = true) override;
		bool isStandBy() override;

		//Basic Tuning
		void setFrequency(const float frequency, const int hiLoForce);
		void setFrequency(const float frequency) override;
		float getFrequency() override;
		unsigned int getIntFrequency() override;
		bool highSide();

		//Audio Qualitiy
		void audioSettings(const bool SNC = false, const bool HCC = true, const bool SM = true);

		unsigned int signalStrength() override;

		void setMute(const bool mute = true) override;
		void setMute(const char side, const bool mute = true);
		bool isMuted(const char side = 'a') override;
		unsigned int getVolume() override;

		bool bassBoosted();

		void setStereo(const bool stereo = true) override;
		bool isStereo() override;			//Stereo Output Set
		bool stereoReception() override;	//Stereo Signal Received

		//Search Settings
		void seek(const unsigned int direction) override;

		void searchLoop(const unsigned int direction, const unsigned int qualityThreshold = 3);
		void searchLoop(const float startFrequency, const unsigned int direction, const unsigned int qualityThreshold);

		void singleSearch(const unsigned int direction, const unsigned int qualityThreshold = 3);
		void singleSearch(const float startFrequency, const unsigned int direction, const unsigned int qualityThreshold);

		void altSearch(const unsigned int direction, const unsigned int qualityThreshold = 3, const float distance = 0.4);
		void altSearch(const float startFrequency, const unsigned int direction, const unsigned int inQualityThreshold, const float distance);

		bool inSearch();

		//Software Programmable Ports
		void setPort(const bool portOne, const bool portTwo, const bool searchIndicator = false);
		bool getPort(const unsigned int port);

		//Radio Data Settings
		bool radioDataEnabled() override;
};

#endif //__TEA5767_HPP