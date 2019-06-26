# Portable Radio
This repository contains several Libraries for a couple of different chips and components. Four in total. They all combine in a very nice way and are perfect to make a Portable Radio with. 
### RDA5807
The RDA5807 is a very complete FM-Radio module. It can do basic Radio-Stuff like setting the frequency, muting, searching and getting the signal strength. But it can also gather Radio Data; send by the Radio Data System.
### TEA5767
The TEA5767 is also an FM-Radio module. It's a little older than the RDA5807, and that's very clear. It doesn't support RDS, the search function is not that great and it's a lot slower in general. It also has way less functionality.
```C++
auto scl = target::pin_oc( target::pins::d8 );
auto sda = target::pin_oc( target::pins::d9 );
auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

auto radio = TEA5767(i2c_bus);
radio.setStereo(true);
radio.audioSettings(true, true, true);
radio.setFrequency(100.7);
for(;;){
  hwlib::cout << radio.signalStrength() << hwlib::endl;
	hwlib::wait_ms(10000);
}
```
### 24CXXX
The 24CXXX Series consists of EEPROM chips that communicate through I2C. They are available in different sizes and universally made. A 24C256 is easilly interchangable with a 24C1024 or 24C16 for example.
### KY040
The famous well known Rotary Encoder is also perfect for a Portable Radio; changing of settings has never been easier. Can be a little tricky without interrupts though.
