/// @file

#ifndef __TEA5767_HPP
#define __TEA5767_HPP

#include "Radio.hpp"

/// \brief
/// TEA5767 Interface
/// \details
/// This is an interface that simplifies the use of TEA5767 chips. Tuning, Muting
/// and setting of Frequencies are some possibilities. It is compatible with the module
/// and breakout, though the breakout has hardwareproblems. It doesn't support searching
/// for channels that well.
class TEA5767 : public Radio{
	private:
		uint8_t data[5] = {0x30, 0x1F, 0x10, 0x10, 0xC0};
		uint8_t status[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
		void setData() override;
		void getStatus() override;
		int testHiLo(float frequency);
		void setPLL(float frequency, int hilo);
	public:
		TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, int bandLimit = 0, uint8_t address = 0x60);
		void setClockFrequency(int frequency = 32);
		void setBandLimit(int limit = 1) override;
		void setFrequency(float frequency = -1, int hiLoForce = -1);
		float getFrequency();
		int signalStrength() override;
		void setMute(bool mute = true) override;
		void setMute(char side, bool mute = true);
		void standBy(bool sleep = true);
		void search(unsigned int direction, int qualityThreshold = 1);
		void search(float startFrequency, unsigned int direction, int qualityThreshold = 1);
		void setStereo(bool stereo = true) override;
		bool stereoReception() override;
		void audioSettings(bool SNC = false, bool HCC = true, bool SM = true);
		void setPort(bool portOne, bool portTwo, bool searchIndicator = false);
		void test();
};

#endif //__TEA5767_HPP