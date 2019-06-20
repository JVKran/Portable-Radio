#ifndef __RDA5807_HPP
#define __RDA5807_HPP

#include "Radio.hpp"

class RDA5807 : public Radio{
	private:
		uint8_t data[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		uint8_t status[10] = {0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00};
		void setData() override;
		void getStatus() override;
		unsigned int channelSpacing;
	public:
		RDA5807(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x10, int bandLimit = 0, unsigned int channelSpacing = 100000);
		unsigned int signalStrength() override;
		void setMute(const bool mute) override;
		void setBandLimit(const unsigned int limit = 0) override;
		void setStereo(const bool stereo = true) override;
		bool stereoReception() override;
		void setSpacing(const unsigned int spacing = 100000);
		void setFrequency(const float frequency);
		unsigned int hasBandLimit();
		void setVolume(unsigned int volume = 15);
		void tune();
		void standBy(const bool standby);
		void normalOutput(const bool normal);
};

#endif //__RDA5807_HPP