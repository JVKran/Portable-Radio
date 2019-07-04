/// @file

#include "hwlib.hpp"
#include "TEA5767.hpp"

/// \brief
/// Test
/// \details
/// This program tests ALL functionalities from the RDA58XX Series chips.
int main( void ){
  namespace target = hwlib::target; 
    
  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto radio = TEA5767(i2c_bus);

  hwlib::cout << "REMEMBER TO UNPLUG RDA5807M BEFORE TESTING SINCE IT WILL MESS UP (responds to 0x60 as well). Tuning to 100.7FM (Q-Music): ";
  radio.setFrequency(100.7);
  hwlib::cout << "DONE" << hwlib::endl << hwlib::boolalpha << "Reception Quality:";

  for(unsigned int i = 0; i < 5; i++){
    hwlib::cout << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << hwlib::endl;
    hwlib::wait_ms(1000);
  }

  radio.setBandLimit();
  hwlib::wait_ms(200);
  hwlib::cout << "Setting Band Limit: " << radio.hasBandLimit() << hwlib::endl;
  radio.setFrequency(100.7);
  hwlib::wait_ms(5000);
  hwlib::cout << "Blocking of tuning to frequency out of range: " << !(radio.getFrequency() == 100.7) << hwlib::endl;
  radio.setBandLimit(0);
  radio.setFrequency(100.7);
  hwlib::cout << "Unsetting of Band Limit: " << (radio.getIntFrequency() == 1006) << hwlib::endl << hwlib::endl;

  hwlib::cout << "Turning All Audio Optimizers On 1 by 1: ";
  radio.audioSettings(true, false, false);
  hwlib::cout << "SNC" << hwlib::endl;
  hwlib::wait_ms(2000);
  radio.audioSettings(true, true, false);
  hwlib::cout << "HCC" << hwlib::endl;
  hwlib::wait_ms(2000);
  radio.audioSettings(true, true, true);
  hwlib::cout << "SM" << hwlib::endl;
  hwlib::wait_ms(2000);
  hwlib::cout << "DONE: " << hwlib::boolalpha << true << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Setting Mono Signal: ";
  radio.setStereo(false);
  hwlib::cout << "DONE: " << hwlib::boolalpha << true << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Setting Stereo Signal: ";
  radio.setStereo(true);
  hwlib::cout << "DONE: " << hwlib::boolalpha << true << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << hwlib::endl << hwlib::endl;
  hwlib::cout << "Tuning to 100.7FM with Low Side Injection: ";
  radio.setFrequency(100.7, 0);
  hwlib::cout << !radio.highSide() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Tuning to 100.7FM with High Side Injection: ";
  radio.setFrequency(100.7, 1);
  hwlib::cout << radio.highSide() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Muting Left Speaker: ";
  radio.setMute('l', true);
  hwlib::cout << "DONE: " << hwlib::boolalpha << radio.isMuted('l') << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Unmuting Left Speaker: ";
  radio.setMute('l', false);
  hwlib::cout << "DONE: " << hwlib::boolalpha << !radio.isMuted('l') << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Turning to Stand-By: ";
  radio.standBy();
  hwlib::cout << hwlib::boolalpha << radio.isStandBy() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Turning back on: ";
  radio.standBy(false);
  hwlib::cout << hwlib::boolalpha << !radio.isStandBy() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Starting Circulair Search-Up: ";
  radio.searchLoop(88, 1, 3);
  hwlib::cout << "Search-Mode Active: " << hwlib::boolalpha << !radio.inSearch() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);
  for(unsigned int i = 0; i < 10; i++){
    radio.searchLoop(1, 3);
    hwlib::cout << "FM-Frequency: " << radio.getIntFrequency() << ", Signal Strength: " << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << hwlib::endl;
    hwlib::wait_ms(4000);
  }
  
  hwlib::cout << "Starting Search-Up: ";
  radio.singleSearch(88, 1, 2);
  hwlib::cout << "Search-Mode Active: " << hwlib::boolalpha << !radio.inSearch() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);
  for(unsigned int i = 0; i < 15; i++){
    radio.singleSearch(1, 3);
    hwlib::cout << "FM-Frequency: " << radio.getIntFrequency() << ", Signal Strength: " << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << hwlib::endl;
    hwlib::wait_ms(4000);
  }

  hwlib::cout << "Starting Alternative Search-Up: ";
  radio.altSearch(88, 1, 3, 0.4);
  hwlib::cout << "Search-Mode Active: " << hwlib::boolalpha << !radio.inSearch() << hwlib::endl << hwlib::endl;
  hwlib::cout << "FM-Frequency: " << radio.getIntFrequency() << ", Signal Strength: " << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << hwlib::endl;
  hwlib::wait_ms(4000);
  for(unsigned int i = 0; i < 10; i++){
    radio.altSearch(1, 3, 0.4);
    hwlib::cout << "FM-Frequency: " << radio.getIntFrequency() << ", Signal Strength: " << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << hwlib::endl;
    hwlib::wait_ms(4000);
  }

}
