#ifndef BIT_HANDLE_H_
#define BIT_HANDLE_H_

//set, clear, toggle, read one bit from a byte
#define		setBit(reg, bit)			reg |= (1 << (bit))
#define		clearBit(reg, bit)			reg &= ~(1 << (bit))
#define		togBit(reg, bit)			reg ^= (1 << (bit))
#define		readBit(reg, bit)			(reg >> (bit)) & 1

//set, clear, toggle group of bits from a byte
#define		setGrpBits(reg, mask)		reg |= (mask)
#define		clearGrpBits(reg, mask)		reg &= ~(mask)
#define		togGrpBits(reg, mask)		reg ^= (mask)

//set, clear, toggle all bits from a byte
#define		setAllBits(reg)				reg = 0xFF
#define		clearAllBits(reg)			reg = 0x00
#define		togAllBits(reg)				reg = reg ^ 0xFF


#endif /* BIT_HANDLE_H_ */