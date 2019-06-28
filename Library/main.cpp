#include "hwlib.hpp"
#include "TEA5767.hpp"
#include "RDA5807.hpp"
#include "GUI.hpp"
#include "KY040.hpp"
#include "A24C256.hpp"

void setTestPresets(A24C256 & memory){
  //4 Presets:
  memory.write(0, 4);

  //Q-Music, 100.7
  memory.write(1, 100);
  memory.write(2, 7);
  char Qdata[]={"Q-Music "};    //Space to make sure no data at next index. Entire memory is filled with testcode.
  memory.write(3, Qdata);

  //Sky Radio 101.2
  memory.write(11, 101);
  memory.write(12, 2);
  char Sdata[]={"SKYRADIO"};
  memory.write(13, Sdata);

  //Midland FM, 107.5
  memory.write(21, 107);
  memory.write(22, 5);
  char Mdata[]={"MIDLAND "};
  memory.write(23, Mdata);

  //Radio 538, 100.1
  memory.write(31, 100);
  memory.write(32, 1);
  char Rdata[]={"RADIO538"};
  memory.write(33, Rdata);
}


int main( void ){
  namespace target = hwlib::target;

//                        Interfaces
//<<<--------------------------------------------------------->>
  auto CLK = hwlib::target::pin_in( hwlib::target::pins::d36 );
  auto DT = hwlib::target::pin_in( hwlib::target::pins::d38 );
  auto SW = hwlib::target::pin_in( hwlib::target::pins::d40 );

  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto radio = RDA5807(i2c_bus);
  radio.begin();

  auto oled = hwlib::glcd_oled( i2c_bus, 0x3C );

  auto button = KY040(CLK, DT, SW);

  auto memory = A24C256(i2c_bus);

//                        Window Parts
//<<<--------------------------------------------------------->>
  auto frequencyArea = hwlib::window_part(oled, hwlib::xy( 0, 0 ), hwlib::xy( 30, 10 ));   
  auto font = hwlib::font_default_8x8();
  //auto textArea = hwlib::terminal_from(frequencyArea, font);

  auto window = hwlib::window_part(oled, hwlib::xy(0, 0), hwlib::xy(128, 64));

  auto stereoWindow = hwlib::window_part(oled, hwlib::xy(0, 54), hwlib::xy(20, 64));
  auto stereoFont = hwlib::font_default_8x8();
  auto stereoField = hwlib::terminal_from(stereoWindow, stereoFont);

  auto timeWindow = hwlib::window_part(oled, hwlib::xy(0, 0), hwlib::xy(40, 10));
  auto timeFont = hwlib::font_default_8x8();
  auto timeField = hwlib::terminal_from(timeWindow, timeFont);

  auto frequencyWindow = hwlib::window_part(oled, hwlib::xy(5, 20), hwlib::xy(128, 50));
  auto frequencyFont = hwlib::font_default_16x16();
  auto frequencyField = hwlib::terminal_from(frequencyWindow, frequencyFont);

  auto stationWindow = hwlib::window_part(oled, hwlib::xy(60, 54), hwlib::xy(128, 64));
  auto stationFont = hwlib::font_default_8x8();
  auto stationField = hwlib::terminal_from(stationWindow, stationFont);

  auto menuWindow = hwlib::window_part(oled, hwlib::xy(0, 40), hwlib::xy(128, 50));
  auto menuFont = hwlib::font_default_8x8();
  auto menuField = hwlib::terminal_from(menuWindow, menuFont);

  auto settingsWindow = hwlib::window_part(oled, hwlib::xy(0, 0), hwlib::xy(128, 64));
  auto settingsFont = hwlib::font_default_8x8();
  auto settingsField = hwlib::terminal_from(settingsWindow, settingsFont);

  auto signalWindow = hwlib::window_part(oled, hwlib::xy(108, 0), hwlib::xy(128, 15));
  auto batteryWindow = hwlib::window_part(oled, hwlib::xy(90, 0), hwlib::xy(105, 10));

  oled.clear();

//                        Initialization
//<<<--------------------------------------------------------->>>
  auto display = GUI(window, oled, button, stereoField, signalWindow, batteryWindow, frequencyField, menuField, settingsField, stationField);

  radio.setFrequency(107.5);
  hwlib::wait_ms(3000);
  button.setPos(0);

  setTestPresets(memory);

//                        Menu Navigation Handling
//<<<-------------------------------------------------------->>>
  bool inPressedArea = false;
  unsigned int iterations = 0;
  int lastKnownPos = 0;
  bool wasPressed = false;
  unsigned int menuArea = 0;      //0 for autoSearch, 1 for manualSearch
  bool bassBoost = false;
  bool showRadioDataStationName = false;
  //bool inPreset = false;

//                        Retrieving Saved Stations from Memory
//<<<-------------------------------------------------------------------------->>>
  unsigned int amountOfPresets = memory.read(0);
  unsigned int curTunedPreset = 0;
  //unsigned int lastCheckedPreset = curTunedPreset - 1; //To force update
  uint8_t newData[] = {"        "};

  std::array<float, 20> stations = {};    //A total of 20 stations can be saved and thus, retrieved.
  //Index 1 contains first whole digit from frequency as int, 2 contains comma number 3 - 10 contain the name. 11 and 12 contain frequency 13 - 20 contain the name. etc.

  for(unsigned int i = 0; i < amountOfPresets; i++){
    stations[i] = float(memory.read(i * 10 + 1) * 10 + (memory.read(i * 10 + 2))) / 10;       //So frequency = 81 + content.
    hwlib::cout << int(stations[i]) << hwlib::endl;
  }

  memory.read(curTunedPreset * 10 + 2, curTunedPreset * 10 + 10, newData);
  char* stationName = (char*)newData;
  display.displayMenuUpdate(radio.signalStrength(), radio.getFrequency() * 10, inPressedArea, 38, radio.stereoReception(), menuArea, radio, showRadioDataStationName, (char*)newData);


  timeField << "14:12" << hwlib::flush;

  menuArea = 0;
  //stations = {90.7, 92.6, 93.4, 94.7, 95.2, 97.6, 98.9, 100.1, 100.7, 101.2, 102.1, 107.5};
  for(;;){
    iterations++;
    button.update();
    while(button.isPressed()){
      button.update();
      hwlib::wait_ms(50);
      wasPressed = true;
    }
    if(wasPressed && menuArea < 3){
      inPressedArea = !inPressedArea;
      //frequencyWindow.clear();  Does not work to get rid of dots. Of by one error in HWLIB
    }
    if(button.getPos() != lastKnownPos){
      //Up in Pressed Area
      if(button.getPos() > lastKnownPos && inPressedArea && menuArea < 3){
        if(menuArea == 0){
          radio.seekChannel(1);
        } else if(menuArea == 1){  //Manual Search
          auto newFrequency = radio.getFrequency() + 0.12;      //Instead of 0.1 to compensate for autotune
          hwlib::wait_ms(30);
          radio.setFrequency(newFrequency);
        } else {
          curTunedPreset+=1;
          hwlib::wait_ms(30);
          radio.setFrequency(stations[curTunedPreset]);
        }
        //Down in Pressed Area
      } else if (inPressedArea && menuArea < 3){
        if(menuArea == 0){
          radio.seekChannel(0);
        } else if(menuArea == 1){  //Manual Search
          auto newFrequency = radio.getFrequency() - 0.12;
          hwlib::wait_ms(30);
          radio.setFrequency(newFrequency);
        } else {
          curTunedPreset-=1;
          hwlib::wait_ms(30);
          radio.setFrequency(stations[curTunedPreset]);
        }
        //Not in Pressed Area
      }

      if(!inPressedArea) {
        if(button.getPos() > lastKnownPos){
          menuArea++;
        } else {
          menuArea--;
        }    //Button Turn outside inPressedArea means change of functionality
        if(menuArea > 6){
          menuArea = 6;
        } else if (menuArea < 0){
          menuArea = 0;
        }
      }
      lastKnownPos = button.getPos();
    }

    /*
    if(menuArea == 2 && wasPressed){
      inPreset = !inPreset;
    }
    */

    if(menuArea == 3 && wasPressed){
      bassBoost = !bassBoost;
      hwlib::wait_ms(30);
      radio.setBassBoost(bassBoost);
    }

    if(menuArea == 4 && wasPressed){
      auto curMute = radio.isMuted();
      hwlib::wait_ms(30);
      radio.setMute(!curMute);
    }

    if(menuArea == 5 && wasPressed){
      auto curRadioData = radio.radioDataEnabled();
      hwlib::wait_ms(30);
      radio.enableRadioData(!curRadioData);
    }

    if(menuArea == 6 && wasPressed){
      showRadioDataStationName = !showRadioDataStationName;
    }

    wasPressed = false;

    if(iterations > 20000){
      iterations = 0;
      button.getPos();
      if(showRadioDataStationName){
        display.displayMenuUpdate(radio.signalStrength(), radio.getFrequency() * 10, inPressedArea, 38, radio.stereoReception(), menuArea, radio, showRadioDataStationName, &radio.radioData.getStationName()[0]);
      } else {
        /*
        if(curTunedPreset != lastCheckedPreset){
          memory.read(curTunedPreset * 10 + 2, curTunedPreset * 10 + 10, newData);
          stationName = (char*)newData;
          lastCheckedPreset = curTunedPreset;
        }
        */
        display.displayMenuUpdate(radio.signalStrength(), radio.getFrequency() * 10, inPressedArea, 38, radio.stereoReception(), menuArea, radio, showRadioDataStationName, (char*)&stationName[0]);
      }
    }
  }

}
