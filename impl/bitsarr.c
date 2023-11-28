#include "bitsarr.h"

// e.g. let arr[2] is bits array  11 is the datalen
// 		then starting form lsb only 11 bits i.e. 8 bits form 1st byte and 3 bits (from lsb) from 
// 		second bytes are valid bits & that holds data


// function return bits from start'th to end'th bit from arr bits array
// maximum 8 bits can be retrive
// ASSUMBTION : requested nunmber of bits are availble in arr to read staring from start bit number
unsigned char bits_get(unsigned char *arr, int start, int len)
{
	unsigned char result = 0, mask;
	int startbyte = start/8, startpos = start % 8;

	len = (len < 8)? len : 8;

	// validating start and end
	if(start > -1)
	{
		mask = (1<<len) - 1;
		mask = mask << startpos;
		result = (arr[startbyte] & mask)>>startpos;
		len -= (8 - startpos); 
	
		if(len > 0)
		{
			mask = (1 <<len) -1;
			result |= (arr[startbyte+1] & mask) << (8 - startpos);
		}
	}

	return result;
}



// write bitscnt number of bits in the bitsarr starting from start bit number in bitsarr
int bits_put(unsigned char *bitsarr, int start, unsigned char *dataarr, int bitscnt)
{
	int startbyte = start/8, startpos = start % 8;
	int space = (8 - startpos), wrbits = 0;
	unsigned char ch = 0;
	
	space = ((bitscnt - wrbits) < space) ? (bitscnt - wrbits):space;
	// here space is min of space avalible(bits) in current byte & bits to be written
	while(wrbits < bitscnt)
	{
		ch = 0;
		ch = bits_get(dataarr, wrbits, space);
		ch = ch << startpos;
		bitsarr[startbyte++] |= ch;

		wrbits += space;

		space = ((bitscnt - wrbits) > 8) ? 8 : (bitscnt - wrbits);
		startpos = 0;
	}
	return wrbits;
}