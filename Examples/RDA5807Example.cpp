#include "hwlib.hpp"
#include "RDA5807.hpp"

//Take a look in the tests folder for even more examples
int main( void ){
  namespace target = hwlib::target; 
    
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
}
