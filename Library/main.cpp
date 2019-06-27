#include "hwlib.hpp"
#include "TEA5767.hpp"
#include "RDA5807.hpp"
#include "GUI.hpp"
#include "KY040.hpp"
#include "A24C256.hpp"

int main( void ){
  namespace target = hwlib::target;

//                        Interfaces
//<<<--------------------------------------------------------->>
  auto CLK = hwlib::target::pin_in( hwlib::target::pins::d36 );
  auto DT = hwlib::target::pin_in( hwlib::target::pins::d38 );
  auto SW = hwlib::target::pin_in( hwlib::target::pins::d40 );
  
  auto button = KY040(CLK, DT, SW);

  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto radio = RDA5807(i2c_bus);
  radio.begin();
  auto oled = hwlib::glcd_oled( i2c_bus, 0x3c );

//                        Window Parts
//<<<--------------------------------------------------------->>
  auto frequencyArea = hwlib::window_part(oled, hwlib::xy( 0, 0 ), hwlib::xy( 30, 10 ));   
  auto font = hwlib::font_default_8x8();
  //auto textArea = hwlib::terminal_from(frequencyArea, font);

  auto window = hwlib::window_part(oled, hwlib::xy(0, 0), hwlib::xy(128, 64));

  auto stereoWindow = hwlib::window_part(oled, hwlib::xy(0, 0), hwlib::xy(20, 10));
  auto stereoFont = hwlib::font_default_8x8();
  auto stereoField = hwlib::terminal_from(stereoWindow, stereoFont);

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

//                        Initialization
//<<<--------------------------------------------------------->>>
  auto display = GUI(window, oled, button, stereoField, signalWindow, batteryWindow, frequencyField, menuField, settingsField);

  radio.setFrequency(101.2);
  hwlib::wait_ms(3000);
  unsigned int iterations = 0;
  int lastKnownPos = 0;
  button.setPos(0);

//                        Menu Navigation Handling
//<<<-------------------------------------------------------->>>
  bool inPressedArea = false;
  bool wasPressed = false;
  unsigned int menuArea = 0;      //0 for autoSearch, 1 for manualSearch
  bool bassBoost = false;
  bool showRadioDataStationName = true;

  //std::array<float, 12> stations = {90.7, 92.6, 93.4, 94.7, 95.2, 97.6, 98.9, 100.1, 100.7, 101.2, 102.1, 107.5};
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
        }
        //Down in Pressed Area
      } else if (inPressedArea && menuArea < 3){
        if(menuArea == 0){
          radio.seekChannel(0);
        } else if(menuArea == 1){  //Manual Search
          auto newFrequency = radio.getFrequency() - 0.12;
          hwlib::wait_ms(30);
          radio.setFrequency(newFrequency);
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

    if(menuArea == 3 && wasPressed){
      bassBoost = !bassBoost;
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

    if(iterations > 2000){
      iterations = 0;
      button.getPos();
      display.displayMenuUpdate(radio.signalStrength(), radio.getFrequency() * 10, inPressedArea, 38, radio.stereoReception(), menuArea, radio, showRadioDataStationName);
      if(showRadioDataStationName){
        //stationField << "\f" << radio.radioData.getStationName() << hwlib::flush;
      } else {

      }
    }
  }

}
