# Portable Radio
This repository contains several Libraries for a couple of different chips and components. Four in total. They all combine in a very nice way and are perfect to make a Portable Radio with. 
### RDA5807 FM-demodulator
The RDA5807 is a very complete FM-Radio module. It can do basic Radio-Stuff like setting the frequency, muting, searching and getting the signal strength. But it can also gather Radio Data; send by the Radio Data System.
```C++
auto scl = target::pin_oc( target::pins::d8 );
auto sda = target::pin_oc( target::pins::d9 );
auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

auto radio = RDA5807(i2c_bus);
radio.begin();
hwlib::cout << "Tuning to 100.7FM: ";
radio.setFrequency(100.7);
for(unsigned int i = 0; i < 8; i++){
    hwlib::wait_ms(5000);
    radio.radioData.update();
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Frequency: " << radio.getIntFrequency() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Station Name: " << radio.radioData.stationName() << hwlib::endl;
    hwlib::wait_ms(3000);
    radio.seekChannel(1);
}
```
### TEA5767 FM-demodulator
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
### 24CXXX EEPROM
The 24CXXX Series consists of EEPROM chips that communicate through I2C. They are available in different sizes and universally made. A 24C256 is easilly interchangable with a 24C1024 or 24C16 for example.
```C++
auto scl = target::pin_oc( target::pins::d8 );
auto sda = target::pin_oc( target::pins::d9 );
auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

auto memory = A24C256(i2c_bus);

char data[]={"Hello World!"};

memory.write(0, data);      //Save data at memory location 0

uint8_t receivedData[12];
memory.read(0, 12, receivedData);
for(unsigned int i = 0; i < 12; i++){
    hwlib::cout << char(receivedData[i]);
}
```
### KY040 Rotary Encoder
The famous well known Rotary Encoder is also perfect for a Portable Radio; changing of settings has never been easier. Can be a little tricky without interrupts though.
```C++
auto CLK = hwlib::target::pin_in( hwlib::target::pins::d22 );
auto DT = hwlib::target::pin_in( hwlib::target::pins::d24 );
auto SW = hwlib::target::pin_in( hwlib::target::pins::d26 );
auto button = KY040(CLK, DT, SW);

for(;;){
	button.update();
	hwlib::cout << button.getPos() << hwlib::endl;
	while(button.isPressed()){
		button.update();
		hwlib::wait_ms(5);
		hwlib::cout << "Pressed" << hwlib::endl;
	}
}
```
### DS3231 Realtime-Clock
This Realtime-Clock is well known about its perfectly tuned clock; it is rated to drift no more than 2 minutes per year. It also supports the setting of alarms and keeping the time up-to-date with its button-cell.
```C++
auto clock = DS3231(i2c_bus);
timeData time;
dateData date;

for(;;){
    time = clock.getTime();
    date = clock.getDate();
    
    hwlib::cout << "Time: " << time.getHours() << ":" << time.getMinutes() << ":" << time.getSeconds() << hwlib::endl;
    hwlib::cout << "Temperature: " << clock.getTemperature() << hwlib::endl;
    hwlib::cout << "Date: " << date.getMonthDay() << "-" << date.getMonth() << "-" << date.getYear() << hwlib::endl << hwlib::endl;
    hwlib::wait_ms(10000);
  }
  ```
