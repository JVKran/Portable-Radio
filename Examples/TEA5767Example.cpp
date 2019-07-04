/// @file

#include "hwlib.hpp"
#include "TEA5767.hpp"

/// \brief
/// Test
/// \details
/// This program tests some functionality of the TEA5767 FM radio module.
/// Take a look in the tests folder for even more examples
int main( void ){
  namespace target = hwlib::target;

  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto radio = TEA5767(i2c_bus);

  radio.setStereo(true);
  radio.audioSettings(true, true, true);

  radio.setFrequency(100.7);

  hwlib::cout << "Tuned Frequency: " << radio.getIntFrequency() << " using ";
  if(radio.highSide()){
    hwlib::cout << "High Side Injection ";
  } else {
    hwlib::cout << "Low Side Injection ";
  }

  hwlib::cout << "resulting in a signal strength of " << radio.signalStrength() << " and a ";

  if(radio.stereoReception()){
    hwlib::cout << "Stereo";
  } else {
    hwlib::cout << "Mono";
  }
  hwlib::cout << " Audio signal." << hwlib::endl;

  for(unsigned int i = 0; i < 3; i++){
      hwlib::cout << radio.signalStrength() << hwlib::endl;
      hwlib::wait_ms(10000);
  }

  radio.setMute(true);
  radio.standBy(true);

  hwlib::cout << "Radio is ";
  if(radio.isStandBy()){
    hwlib::cout << "stand-by";
  } else {
    hwlib::cout << "not in stand-by";
  }
  if(radio.isMuted()){
    hwlib::cout << " and muted" << hwlib::endl;
  } else {
    hwlib::cout << " and unmuted" << hwlib::endl;
  }

  radio.setMute(false);
  radio.standBy(false);

  radio.setMute('r', true); //Mute right speaker

  for(unsigned int i = 0; i < 3; i++){
    radio.searchLoop(1);
    hwlib::wait_ms(2000);
  }
}
