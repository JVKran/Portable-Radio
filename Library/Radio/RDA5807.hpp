#ifndef __RDA5807_HPP
#define __RDA5807_HPP

#include "Radio.hpp"

class RDA5807 : public Radio{
	private:
		unsigned int data[14] = {0x5804, 0x000, 0b11000000, 0b00000011, 0x0000, 0x0000, 0b00001010, 0x0000, 0b10001000, 0b00001111, 0x0000, 0x0000, 0b01000010, 0b00000010};
		unsigned int status[11] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
		unsigned int rdsData[32] = {};
		uint8_t shortData[3] = {};
		const uint8_t indexAddress;
		void setData() override;
		void setData(const int regNumber);
		void setRegister(const int regNumber, const uint16_t value);
		void getStatus() override {}
		unsigned int channelSpacing;
	public:
		int getStatus(const int regNumber);
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