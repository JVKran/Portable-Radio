#ifndef __TEA5767_HPP
#define __TEA5767_HPP

class TEA5767{
	private:
		hwlib::i2c_bus & bus;
		uint8_t address;
		uint8_t data[5] = {0x30, 0x1F, 0x10, 0x10, 0xC0};
		uint8_t status[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
		bool bandLimit;
		void setData();
		void getStatus();
		int testHiLo(float frequency);
		void setHiLo(float frequency, int hilo);
	public:
		TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x60, bool bandLimit = false);
		void setBandLimit(bool limit = true);
		void setFrequency(float frequency = -1, int hiLoForce = -1);
		float getFrequency();
		int signalStrength();
		void setMute(bool mute = true);
		void standBy(bool sleep = true);
		float search(int direction, int qualityThreshold = 3);
		void setStereo(bool stereo = true);
		bool stereoReception();
		void audioSettings(bool SNC, bool HCC, bool SM);
};

#endif //__TEA5767_HPP