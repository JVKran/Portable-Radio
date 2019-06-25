#ifndef __RDA5807_HPP
#define __RDA5807_HPP

#include "Radio.hpp"
#include "radioDataSystem.hpp"

class RDA5807 : public Radio{
	private:
		uint16_t data[8] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x9881, 0x0000, 0x8000};	//First two bytes contain device specific info; are never send but here for completeness.
		unsigned int status[6] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
		uint8_t receivedStatus[12] = {};
		uint8_t shortData[2] = {};
		const uint8_t indexAddress;
		const uint8_t firstReadRegister;
		void setData() override;
		void setData(const int regNumber);
		void setRegister(const int regNumber, const uint16_t value);
		void updateRegister(const int regNumber, const uint16_t value);
		void getStatus() override;
		void getStatus(const uint8_t regNumber);
		float channelSpacing;
		void powerUpEnable(const bool enable);
	public:
		RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, const uint8_t address = 0x10, const uint8_t firstReadRegister = 0x0A, const int bandLimit = 0, const float channelSpacing = 0.1);
		radioDataSystem radioData;
		void begin();
		unsigned int signalStrength() override;
		void setMute(const bool mute = true) override;
		bool isMuted();
		void normalAudio(const bool normal = true);
		void setBassBoost(const bool boost = true);
		bool bassBoosted();
		void setClockSupply(const bool clockSource, const bool directInput = true);
		void setClockFrequency(const unsigned int frequency = 32) override;
		void demodulateMethod(const bool newMethod = true);
		void reset();
		bool isTuned();
		bool isReady();
		bool isStation();
		void seekChannel(const unsigned int direction, const bool wrapContinue = true);
		bool seekCompleted();
		void setSeekThreshold(const uint8_t threshold = 8);
		void setBandLimit(const unsigned int limit = 0) override;
		void setStereo(const bool stereo = true) override;
		bool isStereo() override;
		bool stereoReception() override;
		void setSpacing(const float spacing = 100000);
		bool setFrequency(const float frequency, const bool autoTune = true);
		float getFrequency() override;
		unsigned int getIntFrequency() override;
		unsigned int hasBandLimit() override;
		void setVolume(const uint8_t volume = 15);
		void setTune(const bool tune = true);
		void standBy(const bool sleep = true) override;
		bool isStandBy() override;
		void normalOutput(const bool normal = true);

		void enableRadioData(const bool enable = true);
		bool radioDataReady();
		bool radioDataSynced();
		int radioDataErrors(const int block);
		void printRawRadioData();
		void updateRadioData();
		char* getStationName(const unsigned int dataValidity = 4);
		char* stationName();
		char* getStationText();
		char* stationText();
		void test();
};

#endif //__RDA5807_HPP