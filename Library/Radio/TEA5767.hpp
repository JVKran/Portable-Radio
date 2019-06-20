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
		int testHiLo(const float frequency);
		void setPLL(const float frequency, unsigned int hilo);
	public:
		TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, int bandLimit = 0, uint8_t address = 0x60);
		void setClockFrequency(const unsigned int frequency = 32);
		void setBandLimit(const unsigned int limit = 1) override;
		void setFrequency(const float frequency = -1, const int hiLoForce = -1);
		float getFrequency();
		unsigned int signalStrength() override;
		void setMute(const bool mute = true) override;
		void setMute(const char side, const bool mute = true);
		void standBy(const bool sleep = true);
		void searchLoop(const unsigned int direction, const unsigned int qualityThreshold = 1);
		void singleSearch(const unsigned int direction, const unsigned int qualityThreshold = 1);
		void altSearch(const unsigned int direction, const unsigned int qualityThreshold = 3, const float distance = 0.4);
		void searchLoop(const float startFrequency, const unsigned int direction, const unsigned int qualityThreshold);
		void singleSearch(const float startFrequency, const unsigned int direction, const unsigned int qualityThreshold);
		void altSearch(const float startFrequency, const unsigned int direction, const unsigned int inQualityThreshold, const float distance);
		void setStereo(const bool stereo = true) override;
		bool stereoReception() override;
		void audioSettings(const bool SNC = false, const bool HCC = true, const bool SM = true);
		void setPort(const bool portOne, const bool portTwo, const bool searchIndicator = false);
		bool getPort(const unsigned int port);
		bool inSearch();
		bool isMuted(const char side = 'a');
		bool highSide();
		bool isStandBy();
		unsigned int clockFrequency();
		bool hasBandLimit();
		bool stereo();
};

#endif //__TEA5767_HPP