/// @file

#ifndef __RDA5807_HPP
#define __RDA5807_HPP

#include "Radio.hpp"
#include "radioDataSystem.hpp"

/// \brief
/// RDA5807 Interface
/// \details
/// This is an interface that simplifies the use of RDA5807 chips. Tuning, Muting, Ato-search
/// and Setting of Frequencies are some of its posibilites. It is compatible with all RDA58XX chips,
/// though some RDA58XX chips have no registers for RDS-Data thus not having RDS-abilities.
class RDA5807 : public Radio{
	private:
		//I2C Communication
		uint16_t data[8] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x9881, 0x0000, 0x8000};	//First two bytes contain device specific info; are never send but here for completeness.
		unsigned int status[6] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
		const uint8_t indexAddress;
		const uint8_t firstReadRegister;
		void setData() override;
		void setData(const int regNumber);
		void setRegister(const int regNumber, const uint16_t value);
		void updateRegister(const int regNumber, const uint16_t value);
		void getStatus() override;
		void getStatus(const uint8_t regNumber);

		//Specific Settings
		float channelSpacing;
		void powerUpEnable(const bool enable);
	public:
		RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, const uint8_t address = 0x10, const uint8_t firstReadRegister = 0x0A, const int bandLimit = 0, const float channelSpacing = 0.1);

		//Basic Settings
		void setClockSupply(const bool clockSource, const bool directInput = true);
		void setClockFrequency(const unsigned int frequency = 32) override;

		void begin();
		void reset();

		void setBandLimit(const unsigned int limit = 0) override;
		unsigned int hasBandLimit() override;
		void setSpacing(const float spacing = 100000);

		void standBy(const bool sleep = true) override;
		bool isStandBy() override;

		//Basic Tuning
		bool setFrequency(const float frequency, const bool autoTune = true);
		float getFrequency() override;
		unsigned int getIntFrequency() override;

		void setTune(const bool tune = true);

		void demodulateMethod(const bool newMethod = true);		//Can Improve Signal Strength

		bool isTuned();
		bool isReady();
		bool isStation();

		//Audio Quality
		unsigned int signalStrength() override;

		void normalAudio(const bool normal = true);

		void setBassBoost(const bool boost = true);
		bool bassBoosted();

		void setVolume(const uint8_t volume = 15);
		void setMute(const bool mute = true) override;
		bool isMuted();

		void setStereo(const bool stereo = true) override;
		bool isStereo() override;
		bool stereoReception() override;

		//Search Settings
		void seekChannel(const unsigned int direction, const bool wrapContinue = true);
		bool seekCompleted();
		void setSeekThreshold(const uint8_t threshold = 8);

		//Radio Data System
		radioDataSystem radioData;

		void enableRadioData(const bool enable = true);
		bool radioDataReady();
		bool radioDataSynced();
		int radioDataErrors(const int block);

		char* getStationName(const unsigned int dataValidity = 4);
		char* stationName();
		char* getStationText();
		char* stationText();

		//Raw Radio Data System
		void printRawRadioData();
		void updateRadioData();
};

#endif //__RDA5807_HPP