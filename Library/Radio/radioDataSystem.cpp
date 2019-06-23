#include "RDA5807.hpp"
#include "hwlib.hpp"

void RDA5807::getRDS(){
	getStatus();
	hwlib::cout << status[2] << status[3] << status[4] << status[5] << hwlib::endl;
}

void RDA5807::enableRDS(const bool enable){
	if(enable){
		data[2] |= (1UL << 3);
	} else {
		data[2] &= ~(1UL << 3);
	}
	setData(2);
}

bool RDA5807::rdsReady(){
	getStatus(0);
	return (status[0] >> 15) & 1;
}

bool RDA5807::rdsSync(){
	getStatus(0);
	return (status[0] >> 12) & 1;
}

void RDA5807::processRDS(){
	/*
	getStatus(2);
	getStatus(3);
	getStatus(4);
	getStatus(5);
	*/
	getStatus(); 
	hwlib::cout << char(status[2] >> 8)  << char(status[2] & 0xFF) << ", " << char(status[3] >> 8) << char(status[3] & 0xFF) << ", " << char(status[4] >> 8)  << char(status[4] & 0xFF) << ", " << char(status[5] >> 8) << char(status[5] & 0xFF) <<  hwlib::endl;

}