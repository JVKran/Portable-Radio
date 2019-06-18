#include "hwlib.hpp"
#include "A24C256.hpp"

int main( void ){
    namespace target = hwlib::target; 

    auto scl = target::pin_oc( target::pins::d8 );
    auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto memory = A24C256(i2c_bus);
	auto largeMemory = A24C256(i2c_bus, 512);

	memory.write(40000, 'c');
	hwlib::cout << "Invalid location protection prevents writing on non-existing places: " << hwlib::boolalpha << (int(memory.read(40000)) == 0) << hwlib::endl;

	largeMemory.write(50000, 'c');
	hwlib::cout << "Chips with larger memory have more addresses: " << hwlib::boolalpha << (char(largeMemory.read(50000)) == 'c') << hwlib::endl;

	char data[]={"At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio."};
	memory.write(0, data);

	uint8_t receivedData[353];
	memory.read(0, 353, receivedData);

	for(unsigned int i = 0; i < 353; i++){
		if(char(receivedData[i]) != data[i]){
			hwlib::cout << "Multi-Page Multi-Byte values are written and read correctly: false" << hwlib::endl;
		}
		if(i == 352){
			hwlib::cout << "Multi-Page Multi-Byte values are written and read correctly: true" << hwlib::endl;
		}
	}

	char newData[]={"At vero eos et accusamus."};
	memory.write(0, newData);

	uint8_t newReceivedData[25];
	memory.read(0, 25, newReceivedData);

	for(unsigned int i = 0; i < 25; i++){
		if(char(receivedData[i]) != data[i]){
			hwlib::cout << "Single-Page Multi-Byte values are written and read correctly: false" << hwlib::endl;
		}
		if(i == 24){
			hwlib::cout << "Single-Page Multi-Byte values are written and read correctly: true" << hwlib::endl;
		}
	}

	memory.write(386, 'c');
	hwlib::cout << "Single-Byte values are written and read correctly: " << hwlib::boolalpha << (char(memory.read(386)) == 'c') << hwlib::endl;

	memory.write(32760, data);
	hwlib::cout << "Multi-Byte values are not written if they exceed maximum memory-address: " << hwlib::boolalpha << (char(memory.read(32760)) != 'A') << hwlib::endl;

}