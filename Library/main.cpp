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

  auto menuWindow = hwlib::window_part(oled, hwlib::xy(25, 40), hwlib::xy(80, 50));
  auto menuFont = hwlib::font_default_8x8();
  auto menuField = hwlib::terminal_from(menuWindow, menuFont);

  auto signalWindow = hwlib::window_part(oled, hwlib::xy(108, 0), hwlib::xy(128, 15));
  auto batteryWindow = hwlib::window_part(oled, hwlib::xy(90, 0), hwlib::xy(105, 10));

//                        Initialization
//<<<--------------------------------------------------------->>>
  auto display = GUI(window, oled, button, stereoField, signalWindow, batteryWindow, frequencyField, menuField);

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

  //std::array<float, 12> stations = {90.7, 92.6, 93.4, 94.7, 95.2, 97.6, 98.9, 100.1, 100.7, 101.2, 102.1, 107.5};
  for(;;){
    iterations++;
    button.update();
    while(button.isPressed()){
      button.update();
      hwlib::wait_ms(50);
      wasPressed = true;
    }
    if(wasPressed){
      inPressedArea = !inPressedArea;
      //frequencyWindow.clear();  Does not work to get rid of dots. Of by one error in HWLIB
      wasPressed = false;
    }
    if(button.getPos() != lastKnownPos){
      //Up in Pressed Area
      if(button.getPos() > lastKnownPos && inPressedArea){
        if(menuArea == 0){
          radio.seekChannel(1);
        } else if(menuArea == 1){  //Manual Search
          radio.setFrequency(radio.getFrequency() + 0.1, false);    //Only works with autotune disabled ofcourse
        }
        //Down in Pressed Area
      } else if (inPressedArea){
        if(menuArea == 0){
          radio.seekChannel(0);
        } else if(menuArea == 1){  //Manual Search
          radio.setFrequency(radio.getFrequency() - 0.1, false);    //Only works with autotune disabled ofcourse
        }
        //Not in Pressed Area
      } else {
        if(button.getPos() > lastKnownPos){
          menuArea++;
        } else {
          menuArea--;
        }    //Button Turn outside inPressedArea means change of functionality
      }
      lastKnownPos = button.getPos();
    }
    if(iterations > 20000){
      iterations = 0;
      /*
      display.displayStereo(radio.stereoReception());
      display.batteryPercentage(36);
      display.receptionStrength(radio.signalStrength());
      display.displayFrequency(radio.getFrequency() * 10, inPressedArea);
      */
      display.displayMenuUpdate(radio.signalStrength(), radio.getFrequency() * 10, inPressedArea, 36, radio.stereoReception(), menuArea);
      //stationField << "\f" << radio.radioData.getStationName() << hwlib::flush;
    }
  }

}
