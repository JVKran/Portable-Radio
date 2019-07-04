#include "hwlib.hpp"
#include "TEA5767.hpp"
#include "RDA5807.hpp"
#include "../Application/GUI.hpp"
#include "KY040.hpp"
#include "A24C256.hpp"
#include "DS3231.hpp"

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
  bool displayDebugInfo = true;

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

  auto clock = DS3231(i2c_bus);
  unsigned int lastMinutes = 0;
  timeData time;
  dateData date;

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

  auto frequencyWindow = hwlib::window_part(oled, hwlib::xy(5, 20), hwlib::xy(128, 45));
  auto frequencyFont = hwlib::font_default_16x16();
  auto frequencyField = hwlib::terminal_from(frequencyWindow, frequencyFont);

  auto stationWindow = hwlib::window_part(oled, hwlib::xy(50, 54), hwlib::xy(128, 64));
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

//                        Initialization
//<<<--------------------------------------------------------->>>
  auto display = GUI(window, oled, button, stereoField, signalWindow, batteryWindow, frequencyField, menuField, settingsField, stationField);

  radio.setFrequency(100.7);
  hwlib::wait_ms(3000);
  button.setPos(0);

  setTestPresets(memory);

  auto battery = hwlib::target::pin_adc(0);

//                        Menu Navigation Handling
//<<<-------------------------------------------------------->>>
  bool inPressedArea = false;
  unsigned int iterations = 0;
  int lastKnownPos = 0;
  bool wasPressed = false;
  unsigned int menuArea = 0;      //0 for autoSearch, 1 for manualSearch, 2 for presets, etc.
  bool firstTimeFrequency = false;
  bool bassBoost = false;
  bool showRadioDataStationName = true;
  bool curMute = false;
  bool needToUpdate = false;
  timeData alarmTime = clock.getTime();

//                        Retrieving Saved Stations from Memory
//<<<-------------------------------------------------------------------------->>>
  int amountOfPresets = memory.read(0);
  int curTunedPreset = 0;
  int lastCheckedPreset = curTunedPreset - 1; //To force update
  uint8_t newData[] = {"         "};

  std::array<float, 20> stations = {};    //A total of 20 stations can be saved and thus, retrieved.

  //Index 1 contains first whole digit from frequency as int, 2 contains comma number 3 - 10 contain the name. 11 and 12 contain frequency 13 - 20 contain the name. etc.

  for(int i = 0; i < amountOfPresets; i++){
    stations[i] = float(memory.read(i * 10 + 1) * 10 + (memory.read(i * 10 + 2))) / 10;
  }

  //Display first stationName
  memory.read(curTunedPreset * 10 + 2, 8, newData);
  char* stationName = (char*)newData;
  display.displayMenuUpdate(30, radio.getFrequency() * 10, inPressedArea, 38, false, 1, radio, showRadioDataStationName, (char*)newData, false, clock.getDate());   //Force updates


  time = clock.getTime();
  timeField << time.getHours() << ":" << time.getMinutes() << hwlib::flush;
  for(;;){
    iterations++;
    button.update();
    if(button.isPressed()){
      while(button.isPressed()){
        button.update();
        hwlib::wait_ms(50);
      }
      wasPressed = true;
      needToUpdate = true;
      if(menuArea < 3){
        inPressedArea = !inPressedArea;
      }
      if(displayDebugInfo){
        if(inPressedArea){
          hwlib::cout << "Button has been pressed to change settings in Menu Area " << menuArea << hwlib::endl;
        } else {
          hwlib::cout << "Button has been pressed to stop changing settings in Menu Area " << menuArea << hwlib::endl;
        }
      }
    }
    if(button.getPos() != lastKnownPos){
      needToUpdate = true;
      //Tuned clockwise in Pressed Area
      if(button.getPos() > lastKnownPos && inPressedArea){
        if(displayDebugInfo){
          hwlib::cout << "Turned Clockwise ";
        }
        if(menuArea == 0){    //Auto search
          if(displayDebugInfo){
            hwlib::cout << "to perform Auto Search Up" << hwlib::endl;
          }
          radio.seekChannel(1);
          firstTimeFrequency = true;
          showRadioDataStationName = true;
        } else if(menuArea == 1){  //Manual Search
          showRadioDataStationName = false;
          firstTimeFrequency = true;
          auto newFrequency = radio.getFrequency() + 0.12;      //Instead of 0.1 to compensate for autotune
          hwlib::wait_ms(30);
          radio.setFrequency(newFrequency);
          if(displayDebugInfo){
            hwlib::cout << "to perform Manual Search Up to " << int(newFrequency * 10) << hwlib::endl;
          }
        } else if (menuArea == 2){  //Preset select
          showRadioDataStationName = false;
          curTunedPreset++;
          if(curTunedPreset > amountOfPresets){
            curTunedPreset = 0;
          }
          hwlib::wait_ms(30);
          radio.setFrequency(stations[curTunedPreset]);
          if(displayDebugInfo){
            hwlib::cout << "to select next preset: " << int(stations[curTunedPreset] * 10)<< hwlib::endl;
          }
        }
        //Turned counter clockwise in pressed area
      } else if (inPressedArea){    //Auto search
        if(displayDebugInfo){
          hwlib::cout << "Turned Counter Clockwise ";
        }
        if(menuArea == 0){
          if(displayDebugInfo){
            hwlib::cout << "to perform Auto Search Down" << hwlib::endl;
          }
          firstTimeFrequency = true;
          showRadioDataStationName = true;
          radio.seekChannel(0);
        } else if(menuArea == 1){  //Manual Search
          firstTimeFrequency = true;
          showRadioDataStationName = false;
          auto newFrequency = radio.getFrequency() - 0.1;
          hwlib::wait_ms(30);
          radio.setFrequency(newFrequency);
          if(displayDebugInfo){
            hwlib::cout << "to perform Manual Search Down to " << int(newFrequency * 10) << hwlib::endl;
          }
        } else if (menuArea == 2){  //Preset select
          showRadioDataStationName = false;
          curTunedPreset--;
          if(curTunedPreset < 0){
            curTunedPreset = amountOfPresets - 1;
          }
          hwlib::wait_ms(30);
          radio.setFrequency(stations[curTunedPreset]);
          if(displayDebugInfo){
            hwlib::cout << "to select previous preset: " << int(stations[curTunedPreset] * 10)<< hwlib::endl;
          }
        }
        //Not in Pressed Area
      }

      //If not in pressed area, and the button has been turned, we want to change menuArea
      if(!inPressedArea) {
        if(button.getPos() > lastKnownPos){
          if(displayDebugInfo){
            hwlib::cout << "Turned Clockwise to select Menu Area ";
          }
          if(menuArea == 7){
            menuArea = 0;
          } else {
            menuArea++;
          }
        } else {
          if(displayDebugInfo){
            hwlib::cout << "Turned Counter Clockwise to select Menu Area ";
          }
          if (menuArea == 0){
            menuArea = 7;
          } else{
            menuArea--;
          }
        }
        if(displayDebugInfo){
          hwlib::cout << menuArea << hwlib::endl;
        }
      }
      lastKnownPos = button.getPos();
    }

    //Determine what has to happen when button is pressed in what area.

    if(menuArea == 3 && wasPressed){
      bassBoost = !bassBoost;
      hwlib::wait_ms(30);
      radio.setBassBoost(bassBoost);
      if(displayDebugInfo){
        hwlib::cout << hwlib::boolalpha << "Pressed button to set Bass Boost to: " << bassBoost << hwlib::endl;
      }
    }

    if(menuArea == 4 && wasPressed){
      curMute = !curMute;
      radio.setMute(curMute);
      if(displayDebugInfo){
        hwlib::cout << hwlib::boolalpha << "Pressed button to set Mute to: " << curMute << hwlib::endl;
      }
    }

    if(menuArea == 5 && wasPressed){
      auto curRadioData = radio.radioDataEnabled();
      hwlib::wait_ms(30);
      radio.enableRadioData(!curRadioData);
      if(displayDebugInfo){
        hwlib::cout << hwlib::boolalpha << "Pressed button to set Radio Data Decoding to: " << curRadioData << hwlib::endl;
      }
    }

    if(menuArea == 6 && wasPressed){
      showRadioDataStationName = !showRadioDataStationName;
      if(displayDebugInfo){
        hwlib::cout << hwlib::boolalpha << "Pressed button to set Radio Data Station Name to: " << showRadioDataStationName << hwlib::endl;
      }
    }

    wasPressed = false;

    if(iterations > 200 && needToUpdate){
      battery.refresh();
      needToUpdate = false;
      iterations = 0;
      button.getPos();
      //If it is allowed to show the Radio Data StationName
      if(showRadioDataStationName){
        //And this is the first time this frequency is tuned to it
        if(firstTimeFrequency){
          //Retrieve the name and display it
          stationName = &radio.radioData.getStationName()[0];
          display.displayMenuUpdate(radio.signalStrength(), radio.getFrequency() * 10, inPressedArea, 38, radio.stereoReception(), menuArea, radio, showRadioDataStationName, stationName, curMute, clock.getDate());
          firstTimeFrequency = false;
          if(displayDebugInfo){
            hwlib::cout << "Retrieved Station Name through the Radio Data System: " << stationName << hwlib::endl;
          }
        } else {
          //Just print the already received stationname.
          display.displayMenuUpdate(radio.signalStrength(), radio.getFrequency() * 10, inPressedArea, 38, radio.stereoReception(), menuArea, radio, showRadioDataStationName, (char*)&stationName[0], curMute,clock.getDate());
       }
      } else {
        //If it is a preset, read stationName from memory
        if(curTunedPreset != lastCheckedPreset){
          memory.read(curTunedPreset * 10 + 2, 8, newData);
          stationName = (char*)newData;
          lastCheckedPreset = curTunedPreset;
        }
        if(displayDebugInfo){
          hwlib::cout << "Retrieved Station Name from memory: " << stationName << hwlib::endl;
        }
        display.displayMenuUpdate(radio.signalStrength(), radio.getFrequency() * 10, inPressedArea, 38, radio.stereoReception(), menuArea, radio, showRadioDataStationName, (char*)&stationName[0], curMute, clock.getDate());
      }
      time = clock.getTime();
      if(time.getMinutes() != lastMinutes){
        lastMinutes = time.getMinutes();
        if(time.getHours() < 10){
          timeField << "\f" << "0" << time.getHours();
        } else {
          timeField << "\f" << time.getHours();
        }
        if(lastMinutes < 10){
          timeField << ":0" << time.getMinutes() << hwlib::flush;
        } else {
          timeField << ":" << time.getMinutes() << hwlib::flush;
        }
        if(displayDebugInfo){
          hwlib::cout << hwlib::boolalpha << "Time has been updated to: " << time << hwlib::endl;
        }
      }
    }
  }

}
