# Portable Radio
This repository contains several Libraries for a couple of different chips and components. Four in total. They all combine in a very nice way and are perfect to make a Portable Radio with. A [video](https://www.youtube.com/watch?v=6c8df05tsIs "Portable Radio Showcase") in which I showcase the basic functioning can be found through the given hyperlink.
### Prerequisites
These libraries are written on top of [HWLIB](https://github.com/wovo/hwlib) and [BMPTK](https://github.com/wovo/bmptk). So before you will be able to use these libraries you will have to clone these repositories and set them up. After that you could take a look at the included examples or go straight to programming yourself. It's up to you!
### Hardware
There's actually quite a lot of hardware needed for this project. In basic form it requires the following:
- TEA5767
- RDA5807M
- DS3231
- 24CXX Series EEPROM chip
- Rotary encoder (EC11 or KY040)
- PAM8403 (or other kind of AMP)
- SSD1306 OLED

If you really want it to be portable, you'll also need the following:
- TP4056 Charger
- MT3608 Step-up converter
- LM2596 Step-down converter (for solar panel)
- Switch

All of the above components need to be rated at least 2A since the Amplifier can draw that large amount of current. The wiring is as shown below:
![alt text](/Deliverables/WiringDiagram.PNG "Wiring Diagram")

### Documentation
All code is provided with Doxygen Documentation so there is a general platform to do research after possibilities. When the repository is cloned, one can find the already generated documentation in the [Documentation Folder](/Documentation).
#### Examples
All libraries are provided with [Examples](/Examples) where the basic functionality is shown. I strongly recommend to read the Doxygen Documentations and take a look at the examples for a complete image of the possibilities from this strong combination of libraries.
#### Tests
All libraries have also been tested. If you doubt the correct functioning of the chips you can always perform some tests. These are stored in the Tests folder in each [Library](/Library)
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

auto curTime = clock.getTime();
timeData time;
dateData date;

for(;;){
    time = clock.getTime();
    date = clock.getDate();
    
    hwlib::cout << "Time: " << time.getHours() << ":" << time.getMinutes() << ":" << time.getSeconds() << hwlib::endl;
    hwlib::cout << "Temperature: " << clock.getTemperature() << hwlib::endl;
    hwlib::cout << "Date: " << date.getMonthDay() << "-" << date.getMonth() << "-" << date.getYear() << hwlib::endl << hwlib::endl;
    
    curTime = clock.getTime();
    curTime.setSeconds(curTime.getSeconds() + 10);
    clock.changeFirstAlarm(curTime, dateData(0, 0, 1, 2019));
    clock.setFirstAlarm(14);
    hwlib::cout << "Alarm set, should go in 10 seconds: ";

    hwlib::wait_ms(30);

    while(clock.checkAlarms() == 0){
      hwlib::wait_ms(200);
    }

    hwlib::cout << "Triggered!" << hwlib::endl;
  
  }
  ```
