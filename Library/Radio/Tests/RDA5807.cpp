/// @file

#include "hwlib.hpp"
#include "RDA5807.hpp"

/// \brief
/// Test
/// \details
/// This program tests ALL functionalities from the RDA58XX Series chips.
int main( void ){
  namespace target = hwlib::target; 
    
  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto radio = RDA5807(i2c_bus);
  radio.begin();
  hwlib::cout << "Tuning to 98.9FM (NPO-R2): ";
  radio.setFrequency(98.9);
  hwlib::wait_ms(1000);
  hwlib::cout << "DONE: " << hwlib::boolalpha << (radio.getIntFrequency() == 989) << hwlib::endl << hwlib::boolalpha << "Reception Quality:";
  for(unsigned int i = 0; i < 10; i++){
    hwlib::cout << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << hwlib::endl;
    hwlib::wait_ms(1000);
  }

  hwlib::cout << "Powering Down RDA5807: ";
  radio.standBy(true);
  hwlib::cout << "DONE: " << hwlib::boolalpha << radio.isStandBy() << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Powering Up RDA5807: ";
  radio.standBy(false);
  hwlib::cout << "DONE: " << hwlib::boolalpha << !radio.isStandBy() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Tuning to 100.7FM (Q-Music)" << hwlib::endl;
  radio.setFrequency(100.7);
  hwlib::wait_ms(1000);

  hwlib::cout << "Slowly de- and increasing volume:" << hwlib::endl;
  hwlib::wait_ms(2000);
   for(unsigned int i = 15; i > 0; i--){
    radio.setVolume(i);
    hwlib::cout << i << hwlib::endl;
    hwlib::wait_ms(250);
  }
  for(unsigned int i = 0; i < 15; i++){
    radio.setVolume(i);
    hwlib::cout << i << hwlib::endl;
    hwlib::wait_ms(250);
  }

  hwlib::cout << "New Demodulate Method: ";
  radio.demodulateMethod(true);
  hwlib::cout << "Signal Strength: " << hwlib::boolalpha << radio.signalStrength() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Usual Demodulate Method: ";
  radio.demodulateMethod(false);
  hwlib::cout << "Signal Strength: " << hwlib::boolalpha << radio.signalStrength() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Setting Mono Signal: ";
  radio.setStereo(false);
  hwlib::cout << "DONE: " << hwlib::boolalpha << !radio.isStereo() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Setting Stereo Signal: ";
  radio.setStereo(true);
  hwlib::cout << "DONE: " << hwlib::boolalpha << true << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Muting: ";
  radio.setMute(true);
  hwlib::cout << "DONE: " << hwlib::boolalpha << true << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Unmuting: ";
  radio.setMute(false);
  hwlib::cout << "DONE: " << hwlib::boolalpha << !radio.isMuted() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Bass-Boost On: ";
  radio.setBassBoost(true);
  hwlib::cout << "DONE: " << hwlib::boolalpha << radio.bassBoosted() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Bass-Boost Off: ";
  radio.setBassBoost(false);
  hwlib::cout << "DONE: " << hwlib::boolalpha << !radio.bassBoosted() << hwlib::endl << hwlib::endl;
  hwlib::wait_ms(2000);

  hwlib::cout << "Starting Search-Up: ";
  hwlib::wait_ms(2000);
  for(unsigned int i = 0; i < 10; i++){
    radio.seekChannel(1);
    hwlib::cout << "FM-Frequency: " << int(radio.getFrequency()) << hwlib::endl;
    hwlib::wait_ms(2000);
    hwlib::cout << "Seek Completed: " << radio.seekCompleted() << ", Signal Strength: " << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << ", Station: " << radio.isStation() << ", Tuned: " << radio.isTuned() << hwlib::endl << hwlib::endl;    hwlib::wait_ms(3000);
    hwlib::wait_ms(3000);
  }

  hwlib::cout << hwlib::endl << "Starting Search-Down: ";
  hwlib::wait_ms(2000);
  for(unsigned int i = 0; i < 10; i++){
    radio.seekChannel(0);
    hwlib::cout << "FM-Frequency: " << int(radio.getFrequency()) << hwlib::endl;
    hwlib::wait_ms(2000);
    hwlib::cout << "Seek Completed: " << radio.seekCompleted() << ", Signal Strength: " << radio.signalStrength() << ", Stereo: " << radio.stereoReception() << ", Station: " << radio.isStation() << ", Tuned: " << radio.isTuned() << hwlib::endl << hwlib::endl;
    hwlib::wait_ms(3000);
  }

  radio.setFrequency(100.7);
  hwlib::cout << hwlib::endl << "Starting Reading of Radio Data System: ";
  for(unsigned int i = 0; i < 8; i++){
    hwlib::wait_ms(5000);
    radio.radioData.update();
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Frequency: " << radio.getIntFrequency() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Station Name: " << radio.radioData.stationName() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Radio Text: " << radio.radioData.stationText() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Country Code: " << radio.radioData.getCountryCode() << hwlib::endl;      //0xFFFF where F is one nibble
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Area Coverage: " << radio.radioData.getProgramArea() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Refrence Number: " << radio.radioData.getProgramRefrence() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Message Group Type: " << radio.radioData.getMessageGroupType() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Traffic Announcement: " << radio.radioData.trafficAnnouncement() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Music Playing: " << radio.radioData.currentMusic() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Clear Screen Request: " << radio.radioData.clearScreen() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Program Type: " << radio.radioData.getProgramType() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Static Program Type: " << radio.radioData.staticProgramType() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Stereo Broadcast: " << radio.radioData.stereo() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Compressed Broadcast: " << radio.radioData.compressed() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Emergency Warning: " << radio.radioData.emergencyWarning() << hwlib::endl;
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Received Time: " << radio.radioData.hours() << ":" << radio.radioData.minutes() << hwlib::endl;
    hwlib::cout << hwlib::endl << hwlib::endl;
    hwlib::wait_ms(3000);
    radio.seekChannel(1);
  }

  for(unsigned int i = 0; i < 5; i++){
    hwlib::wait_ms(5000);
    radio.seekChannel(1);
    hwlib::wait_ms(2000);   //Give time to complete tune process
    radio.radioData.rawData();
  }

  radio.setFrequency(107.5);    //No station Supports RDS-A, this one is RDS-A but doesn't send time...
  for(;;){
    hwlib::wait_ms(50);
    radio.radioData.update();
    hwlib::cout << radio.radioData.hours() << ":" << radio.radioData.minutes() << hwlib::endl;
  }

}
