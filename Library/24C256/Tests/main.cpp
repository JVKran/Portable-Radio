/// @file

#include "hwlib.hpp"
#include "A24C256.hpp"

/// \brief
/// Test
/// \details
/// This program tests ALL functionality from the I2C EEPROM memory chips.
int main( void ){
  namespace target = hwlib::target; 

  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto writeProtectPin = target::pin_in_out( target::pins::d3 );

  hwlib::wait_ms(1000);   //Wait for terminal

  auto memory = A24C256(i2c_bus, 256, 0x50, writeProtectPin);
  auto largeMemory = A24C256(i2c_bus, 512);
  auto addressMemory = A24C256(i2c_bus, 256, 0x10);
  auto falseMemory = A24C256(i2c_bus, 230);

  hwlib::cout << hwlib::boolalpha << hwlib::setw(100) << hwlib::left << "Initizalisation with valid parameters: " << ((addressMemory.getAddress() == 0x10) && (addressMemory.getMemorySize() == 256) ) << hwlib::endl;
  hwlib::cout << hwlib::boolalpha << hwlib::setw(100) << hwlib::left << "Initizalisation with correction of invalid parameters: " << ((falseMemory.getAddress() == 0x50) && (falseMemory.getMemorySize() == 256) ) << hwlib::endl;

  memory.write(40000, 'c');
  hwlib::cout << hwlib::setw(100) << hwlib::left << "Invalid location protection prevents writing and reading on and from non-existing places: " << hwlib::boolalpha << (int(memory.read(40000)) == 0) << hwlib::endl;

  largeMemory.write(50000, 'c');
  hwlib::cout << hwlib::setw(100) << hwlib::left << "Chips with larger memory have more addresses: " << hwlib::boolalpha << (char(largeMemory.read(50000)) == 'c') << hwlib::endl;
  
  
  char data[]={"At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio."};
  memory.write(0, data);

  uint8_t receivedData[353];
  memory.read(0, 353, receivedData);

  hwlib::cout << hwlib::endl << "Saved Text: " << hwlib::endl;

  for(unsigned int i = 0; i < 353; i++){
    hwlib::cout << char(receivedData[i]);
    if(char(receivedData[i]) != data[i]){
      hwlib::cout << hwlib::endl << hwlib::endl << hwlib::setw(100) << hwlib::left << "Multi-Page Multi-Byte values are written and read correctly: " << false << hwlib::endl;
    }
    if(i == 352){
      hwlib::cout << hwlib::endl << hwlib::endl << hwlib::setw(100) << hwlib::left << "Multi-Page Multi-Byte values are written and read correctly: " << true << hwlib::endl;
    }
  }

  char newData[]={"At vero eos et accusamus."};
  memory.write(0, newData);

  uint8_t newReceivedData[25];
  memory.read(0, 25, newReceivedData);

  for(unsigned int i = 0; i < 25; i++){
    if(char(receivedData[i]) != data[i]){
      hwlib::cout << hwlib::setw(100) << hwlib::left  << "Single-Page Multi-Byte values are written and read correctly: " << false << hwlib::endl;
    }
    if(i == 24){
      hwlib::cout << hwlib::setw(100) << hwlib::left << "Single-Page Multi-Byte values are written and read correctly: " << true << hwlib::endl;
    }
  }

  memory.write(386, 'c');
  hwlib::cout << hwlib::setw(100) << hwlib::left << "Single-Byte values are written and read correctly: " << hwlib::boolalpha << (char(memory.read(386)) == 'c') << hwlib::endl;

  memory.write(32760, data);
  hwlib::cout << hwlib::setw(100) << hwlib::left << "Multi-Byte values are not written if they exceed maximum memory-address: " << hwlib::boolalpha << (char(memory.read(32760)) != 'A') << hwlib::endl;

  memory.write(300, 'c');
  memory.setWriteProtect();
  hwlib::cout << hwlib::setw(100) << hwlib::left << "Write-Protect Enabled: " << memory.getWriteProtect() << hwlib::endl;
  memory.write(300, 'z');
  hwlib::cout << hwlib::setw(100) << hwlib::left << "Writing impossible when Write Protection Enabled " << hwlib::boolalpha << (char(memory.read(300)) == 'c') << hwlib::endl;

  memory.setWriteProtect(false);
  memory.write(300, 'z');
  hwlib::cout << hwlib::setw(100) << hwlib::left << "Writing possible again when Write Protection Disabled " << hwlib::boolalpha << (char(memory.read(300)) == 'z') << hwlib::endl;

}