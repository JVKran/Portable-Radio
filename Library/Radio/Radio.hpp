/// @file

#ifndef __RADIO_HPP
#define __RADIO_HPP

/// \brief
/// Abstract Base Class Radio
/// \details
/// This is an abstract base class which supports all basic functions radios have. In pariticular,
/// it supports the TEA5767 and RDA58XX Series. RDA58XX Chips also have a compatibility mode in which 
/// they support basic functionality of the TEA5767 (addres: 0x60). Thus the TEA5767 Methods are alco compatible with 
/// the RDA58XX Series.
class Radio{
	protected:
		//I2C Communication
		hwlib::i2c_bus & bus;
		const uint8_t address;
		virtual void setData() = 0;
		virtual void getStatus() = 0;
		//Important For Legal Frequencies
		int bandLimit;
	public:
		Radio(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x60, int bandLimit = 0);

		//Basic Settings
		virtual void setClockFrequency(const unsigned int frequency = 32) = 0;
		virtual unsigned int getClockFrequency() = 0;

		virtual void setBandLimit(const unsigned int limit = 0) = 0;
		virtual unsigned int hasBandLimit() = 0;

		virtual void standBy(const bool sleep = true) = 0;
		virtual bool isStandBy() = 0;

		//Basic Tuning
		virtual void setFrequency(const float frequency = -1) = 0;
		virtual float getFrequency() = 0;
		virtual unsigned int getIntFrequency() = 0;

		//Audio Quality
		virtual unsigned int signalStrength() = 0;

		virtual void setMute(const bool mute) = 0;
		virtual bool isMuted(const char side = 'a') = 0;
		virtual unsigned int getVolume() = 0;

		virtual bool bassBoosted() = 0;

		virtual void setStereo(const bool stereo = true) = 0;
		virtual bool stereoReception() = 0;
		virtual bool isStereo() = 0;

		//Search Settings
		virtual void seek(const unsigned int direction) = 0;

		//Radio Data Settings
		virtual bool radioDataEnabled() = 0;
};

#endif //__RADIO_HPP