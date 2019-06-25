#ifndef __RADIO_HPP
#define __RADIO_HPP

class Radio{
	protected:
		hwlib::i2c_bus & bus;
		int bandLimit;
		const uint8_t address;
		virtual void setData() = 0;
		virtual void getStatus() = 0;
	public:
		Radio(hwlib::i2c_bus_bit_banged_scl_sda & bus, int bandLimit = 0, uint8_t address = 0x60);
		virtual unsigned int signalStrength() = 0;
		virtual void setMute(const bool mute) = 0;
		virtual void setBandLimit(const unsigned int limit = 0) = 0;
		virtual void setStereo(const bool stereo = true) = 0;
		virtual bool stereoReception() = 0;
		virtual bool isStereo() = 0;
		virtual float getFrequency() = 0;
		virtual unsigned int getIntFrequency() = 0;
		virtual void standBy(const bool sleep = true) = 0;
		virtual bool isStandBy() = 0;
		virtual void setClockFrequency(const unsigned int frequency = 32) = 0;
		virtual unsigned int hasBandLimit() = 0;
};

#endif //__RADIO_HPP