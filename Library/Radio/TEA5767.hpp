#ifndef __TEA5767_HPP
#define __TEA5767_HPP

#include "Radio.hpp"

class TEA5767 : public Radio{
	private:
		uint8_t data[5] = {0x30, 0x1F, 0x10, 0x10, 0xC0};
		uint8_t status[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
		void setData() override;
		void getStatus() override;
		int testHiLo(float frequency);
		void setHiLo(float frequency, int hilo);
	public:
		TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, int bandLimit = 0, uint8_t address = 0x60);
		void setBandLimit(int limit = 0) override;
		void setFrequency(float frequency = -1, int hiLoForce = -1);
		float getFrequency();
		int signalStrength() override;
		void setMute(bool mute = true) override;
		void standBy(bool sleep = true);
		void search(int direction, int qualityThreshold = 1);
		void setStereo(bool stereo = true) override;
		bool stereoReception() override;
		void audioSettings(bool SNC, bool HCC, bool SM);
};

#endif //__TEA5767_HPP