#ifndef __RDA5807_HPP
#define __RDA5807_HPP

#include "Radio.hpp"

class RDA5807 : public Radio{
	private:
		uint16_t data[8] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};	//First two bytes contain device specific info; are never send but here for completeness.
		unsigned int status[6] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
		uint8_t shortData[3] = {};
		const uint8_t indexAddress;
		void setData() override;
		void setData(const int regNumber);
		void setRegister(const int regNumber, const uint16_t value);
		void updateRegister(const int regNumber, const uint16_t value);
		void getStatus() override;
		void getStatus(const uint8_t regNumber);
		float channelSpacing;
	public:
		RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x10, int bandLimit = 0, float channelSpacing = 0.1);
		void begin();
		unsigned int signalStrength() override;
		void setMute(const bool mute) override;
		void normalAudio(const bool normal = true);
		void setBassBoost(const bool boost = true);
		void setClockSupply(const bool clockSource, const bool directInput);
		void setClockFrequency(const unsigned int frequency);
		void demodulateMethod(const bool newMethod);
		void reset();
		bool isTuned();
		bool isReady();
		bool isStation();
		void powerUpEnable(const bool enable);
		void seekChannel(const unsigned int direction, const bool wrapContinue = true);
		void setBandLimit(const unsigned int limit = 0) override;
		void setStereo(const bool stereo = true) override;
		bool stereoReception() override;
		void setSpacing(const float spacing = 100000);
		bool setFrequency(const float frequency, const bool autoTune = true);
		float getFrequency();
		unsigned int hasBandLimit();
		void setVolume(const uint8_t volume = 15);
		void tune(const bool tune);
		void standBy(const bool standby);
		void normalOutput(const bool normal);
		bool rdsReady();
		void getRDS();
		void processRDS();
		void enableRDS(const bool enable = true);
		bool rdsSync();
		void test();
};

#endif //__RDA5807_HPP