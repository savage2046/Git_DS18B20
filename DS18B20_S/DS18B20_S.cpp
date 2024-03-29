/*
Base on OneWire v2.2
Edit by savage 2014-05-07

For single senser only

*/

#include "DS18B20_S.h"

DS18B20_S::DS18B20_S(uint8_t pin)
{
	PIN=pin;
	pinMode(pin, OUTPUT);
	digitalWrite(PIN,1);
	delayMicroseconds(100);//初始化的时候，先要保证以高电平开始,顺便充电
  set(0x3F);//设定为10位模式（默认是12位模式）
}

void DS18B20_S::set(byte n){
	reset();	
	write(0xCC);//skip	
	write(0x4E);//write
	write(0);//Th
	write(0);//Tl
	write(n);//seting 10bit mode B00111111 10位模式
}	

// Returns 1 if a device asserted a presence pulse, 0 otherwise.
//
uint8_t DS18B20_S::reset()	
{		
	uint8_t r;
	pinMode(PIN,OUTPUT);
	digitalWrite(PIN,0);
	delayMicroseconds(480);//拉低电位至少480um，reset	
	pinMode(PIN,INPUT);	
		
	delayMicroseconds(70);		
	r=!digitalRead(PIN);//传感器回应
	delayMicroseconds(410);
	return r;
}

//
// Write a bit. Port and bit is used to cut lookup time and provide
// more certain timing.
//
void DS18B20_S::write_bit(uint8_t v)
{
  pinMode(PIN,OUTPUT);  
	if (v & 1) {		//写1			
    digitalWrite(PIN,0);
		delayMicroseconds(5);
		digitalWrite(PIN,1);		
		delayMicroseconds(55);
	} else {     //写0		
		digitalWrite(PIN,0);
		delayMicroseconds(60);		
    digitalWrite(PIN,1);    
		delayMicroseconds(5);
	}
}
//
// Read a bit. Port and bit is used to cut lookup time and provide
// more certain timing.
//
uint8_t DS18B20_S::read_bit()
{
	uint8_t r;	
	pinMode(PIN,OUTPUT);	
	digitalWrite(PIN,0);//拉低电位1us以上
	delayMicroseconds(2);
	pinMode(PIN,INPUT);     	
	delayMicroseconds(10);// let pin float, pull up will raise
	r = digitalRead(PIN);//必须在15us内采样	
	delayMicroseconds(50);
	return r;
}
//写
void DS18B20_S::write(uint8_t v) {	  	
    uint8_t bitMask;
    for (bitMask = 0x01; bitMask; bitMask <<= 1) {
	  DS18B20_S::write_bit( (bitMask & v)?1:0);
	  	    }    
 }

// Read 
uint8_t DS18B20_S::read() {	  	
    uint8_t bitMask;
    uint8_t r = 0;
    for (bitMask = 0x01; bitMask; bitMask <<= 1) {
	  if ( DS18B20_S::read_bit()) r |= bitMask;
    }    
    return r;
}

void DS18B20_S::start(){
	 reset();
   write(0xCC);//skip
   write(0x44);
}

boolean DS18B20_S::ready(){
	  return read()&0x01;
}

float DS18B20_S::get(){	
	reset();	
	write(0xCC);//
	write(0xBE);
	byte data[9];
	byte i;
	for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = read();
  }
  if(crc8(data, 8)!=data[8])return 250;//crc 错误
  int raw = (data[1] << 8) | data[0];	
  return raw*0.0625;  		
}


#if DS18B20_S_CRC
// The 1-Wire CRC scheme is described in Maxim Application Note 27:
// "Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products"
// Compute a Dallas Semiconductor 8 bit CRC directly.
// this is much slower, but much smaller, than the lookup table.
//
uint8_t DS18B20_S::crc8(const uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;	
	while (len--) {
		uint8_t inbyte = *addr++;
		for (uint8_t i = 8; i; i--) {
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) crc ^= 0x8C;
			inbyte >>= 1;
		}
	}
	return crc;
}
#endif