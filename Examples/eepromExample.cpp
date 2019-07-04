#include "hwlib.hpp"
#include "A24C256.hpp"

int main( void ){
  namespace target = hwlib::target; 

  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto writeProtectPin = hwlib::target::pin_in_out ( target::pins::d3 );

  auto memory = A24C256(i2c_bus, 256, 0x50, writeProtectPin);
  auto largeMemory = A24C256(i2c_bus, 512);
  auto repairedMemory = A24C256(i2c_bus, 1000);   //Is automatically resized to 1024.

  hwlib::wait_ms(2000);   //Wait for Terminal

  //Write one single byte at location 0 and retrieve it
  memory.write(0, 'c');
  hwlib::cout << char(memory.read(0)) << hwlib::endl;

  //Write 12 bytes to location 20
  char data[]={"Hello World!"};
  memory.write(20, data);

  //Retrieve multiple bytes (efficient way)
  uint8_t receivedData[12];
  memory.read(20, 12, receivedData);

  //Print received bytes
  for(unsigned int i = 0; i < 12; i++){
    hwlib::cout << char(receivedData[i]);
  }
  hwlib::cout << hwlib::endl;

  //Another (less efficient) way to retrieve multiple bytes
  for(unsigned int i = 20; i < 32; i++){
    hwlib::cout << char(memory.read(i));
  }
  hwlib::cout << hwlib::endl;

  //Write integer 100 to address 0 (since a uint8_t is written, the max size of the integer is 255)
  memory.write(0, 100);
  hwlib::cout << memory.read(0) << hwlib::endl;

  //Way to store a boolean
  memory.write(0, true);
  hwlib::cout << (bool(memory.read(0)) && 1);

  //Overwriting page-boundaries is taken care of.
  char newData[]={"At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio."};
  memory.write(0, newData);

  uint8_t newReceivedData[353];
  memory.read(0, 353, newReceivedData);

  hwlib::cout << hwlib::endl << "Saved Text: " << hwlib::endl;

  for(unsigned int i = 0; i < 353; i++){
    hwlib::cout << char(newReceivedData[i]);
  }

  //Making pin WP high, disables its functionality to save given data; it protects saved data.
  memory.write(300, 'c');
  memory.setWriteProtect();
  hwlib::cout << hwlib::setw(100) << hwlib::left << hwlib::boolalpha << "Write-Protect Enabled: " << memory.getWriteProtect() << hwlib::endl;
  memory.write(300, 'z');
  hwlib::cout << hwlib::setw(100) << hwlib::left << "Writing impossible when Write Protection Enabled " << hwlib::boolalpha << (char(memory.read(300)) == 'c') << hwlib::endl;


}