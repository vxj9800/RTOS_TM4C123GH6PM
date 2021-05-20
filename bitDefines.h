//  Author: Vatsal Asitkumar Joshi
//  Date: Sep 6th, 2019
//  This code changes the LED on the device from red to green to blue to red again.
//
//  If you are done writing the code, now is a good time to debug it.
//

#ifndef BITDEFINES_H_
#define BITDEFINES_H_

// Macros that can be used to set a specific bit
#define bit0    0x1
#define bit1    0x2
#define bit2    0x4
#define bit3    0x8
#define bit4    0x10
#define bit5    0x20
#define bit6    0x40
#define bit7    0x80
#define bit8    0x100
#define bit9    0x200
#define bit10   0x400
#define bit11   0x800
#define bit12   0x1000
#define bit13   0x2000
#define bit14   0x4000
#define bit15   0x8000
#define bit16   0x10000
#define bit17   0x20000
#define bit18   0x40000
#define bit19   0x80000
#define bit20   0x100000
#define bit21   0x200000
#define bit22   0x400000
#define bit23   0x800000
#define bit24   0x1000000
#define bit25   0x2000000
#define bit26   0x4000000
#define bit27   0x8000000
#define bit28   0x10000000
#define bit29   0x20000000
#define bit30   0x40000000
#define bit31   0x80000000
#define bit32	0x100000000

//	Macro to convert a register and bit number to respective bitband alias
#define peripheralBitBand(regAddress,bitNumber) (*((volatile uint32_t *) (0x42000000 + ((uint32_t)&regAddress-0x40000000)*32 + bitNumber*4)))

#define setRegisterBit(regAddress,bitNumber)	regAddress |= (bitNumber)

#define clrRegisterBit(regAddress,bitNumber)	regAddress &= (~(bitNumber))

#define flpRegisterBit(regAddress,bitNumber)	regAddress ^= (bitNumber)

#define setRegisterVal(regAddress,value)	regAddress = value

#endif /* BITDEFINES_H_ */
