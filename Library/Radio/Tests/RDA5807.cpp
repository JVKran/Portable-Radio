#include "hwlib.hpp"
#include "RDA5807.hpp"

int main( void ){
  namespace target = hwlib::target; 
    
  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto radio = RDA5807(i2c_bus);
  radio.begin();
  hwlib::cout << "Tuning to 100.7FM (Q-Music): ";
  radio.setFrequency(100.7);
  hwlib::cout << "DONE" << hwlib::endl << hwlib::boolalpha << "Reception Quality:";

  for(unsigned int i = 0; i < 10; i++){
    hwlib::cout << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << hwlib::endl;
    hwlib::wait_ms(1000);
  }

  hwlib::cout << "Setting Mono Signal: ";
  radio.setStereo(false);
  hwlib::cout << "DONE: " << hwlib::boolalpha << !radio.stereoReception() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Setting Stereo Signal: ";
  radio.setStereo(true);
  hwlib::cout << "DONE: " << hwlib::boolalpha << radio.stereoReception() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Muting: ";
  radio.setMute(true);
  hwlib::cout << "DONE: " << hwlib::boolalpha << radio.isMuted() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Unmuting: ";
  radio.setMute(false);
  hwlib::cout << "DONE: " << hwlib::boolalpha << !radio.isMuted() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Starting Search-Up: ";
  hwlib::wait_ms(2000);
  for(unsigned int i = 0; i < 10; i++){
    radio.seekChannel(1);
    hwlib::cout << "FM-Frequency: " << int(radio.getFrequency()) << ", Signal Strength: ";
    hwlib::wait_ms(2000);
    hwlib::cout << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << hwlib::endl;
    hwlib::wait_ms(2000);
  }

}
