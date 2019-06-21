#ifndef __RDA5807_HPP
#define __RDA5807_HPP

#include "Radio.hpp"

class RDA5807 : public Radio{
	private:
		unsigned int data[8] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
		unsigned int status[6] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
		uint8_t shortData[3] = {};
		const uint8_t indexAddress;
		void setData() override;
		void setData(const int regNumber);
		void setRegister(const int regNumber, const uint16_t value);
		void getStatus() override;
		void getStatus(const int regNumber);
		unsigned int channelSpacing;
	public:
		RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x10, int bandLimit = 0, unsigned int channelSpacing = 100000);
		unsigned int signalStrength() override;
		void setMute(const bool mute) override;
		void setBandLimit(const unsigned int limit = 0) override;
		void setStereo(const bool stereo = true) override;
		bool stereoReception() override;
		void setSpacing(const unsigned int spacing = 100000);
		void setFrequency(int frequency);
		unsigned int hasBandLimit();
		void setVolume(unsigned int volume = 15);
		void tune(const bool tune);
		void standBy(const bool standby);
		void normalOutput(const bool normal);
		void setBassBoost(const bool boost);
		void init();
};

#endif //__RDA5807_HPP