/*
 * SI324regs.h
 *
 *  Created on: 29 Sep 2018
 *      Author: gkasp
 */

typedef struct SI324Info
{
	unsigned char RegIndex;	/* Register Number */
	unsigned char Value;		/* Value to be Written */
} SI324Info;

	SI324Info InitTable[] = {
		{  0, 0x34},	/* Register 0 */
		{  1, 0xE4},	/* Register 1 */
		{  2, 0xA2},	/* Register 2 */
		{  3, 0x55},	/* Register 3 */
		{  4, 0x92},	/* Register 4 */
		{  5, 0xAD},	/* Register 5 */
		{  6, 0x3F},	/* Register 6 */
		{  7, 0x2A},	/* Register 7 */
		{  8, 0x00},	/* Register 8 */
		{  9, 0xC0},	/* Register 9 */
		{ 10, 0x00},	/* Register 10 */
		{ 11, 0x40},	/* Register 11 */
		{ 19, 0x29},	/* Register 19 */
		{ 20, 0x3E},	/* Register 20 */
		{ 21, 0xFE},	/* Register 21 */
		{ 22, 0xDF},	/* Register 22 */
		{ 23, 0x1F},	/* Register 23 */
		{ 24, 0x3F},	/* Register 24 */
		{ 25, 0x60},	/* Register 25 */
		{ 31, 0x00},	/* Register 31 */
		{ 32, 0x00},	/* Register 32 */
		{ 33, 0x07},	/* Register 33 */
		{ 34, 0x00},	/* Register 34 */
		{ 35, 0x00},	/* Register 35 */
		{ 36, 0x07},	/* Register 36 */
		{ 40, 0x80},	/* Register 40 */
		{ 41, 0x01},	/* Register 41 */
		{ 42, 0x6B},	/* Register 42 */
		{ 43, 0x00},	/* Register 43 */
		{ 44, 0x00},	/* Register 44 */
		{ 45, 0x0C},	/* Register 45 */
		{ 46, 0x00},	/* Register 46 */
		{ 47, 0x00},	/* Register 47 */
		{ 48, 0x0C},	/* Register 48 */
		{ 55, 0x00},	/* Register 55 */
		{131, 0x1F},	/* Register 131 */
		{132, 0x02},	/* Register 132 */
		{137, 0x01},	/* Register 137 */
		{138, 0x0F},	/* Register 138 */
		{139, 0xFF},	/* Register 139 */
		{142, 0x00},	/* Register 142 */
		{143, 0x00},	/* Register 143 */
		{136, 0x40}		/* Register 136 */





//		100MHz free run
//				{  0, 0x74},	/* Register 0 */
//				{  1, 0xE1},	/* Register 1 */
//				{  2, 0x32},	/* Register 2 */
//				{  3, 0x55},	/* Register 3 */
//				{  4, 0x92},	/* Register 4 */
//				{  5, 0xAD},	/* Register 5 */
//				{  6, 0x3F},	/* Register 6 */
//				{  7, 0x2A},	/* Register 7 */
//				{  8, 0x00},	/* Register 8 */
//				{  9, 0xC0},	/* Register 9 */
//				{ 10, 0x00},	/* Register 10 */
//				{ 11, 0x40},	/* Register 11 */
//				{ 19, 0x29},	/* Register 19 */
//				{ 20, 0x3E},	/* Register 20 */
//				{ 21, 0xFE},	/* Register 21 */
//				{ 22, 0xDF},	/* Register 22 */
//				{ 23, 0x1F},	/* Register 23 */
//				{ 24, 0x3F},	/* Register 24 */
//				{ 25, 0xA0},	/* Register 25 */
//				{ 31, 0x00},	/* Register 31 */
//				{ 32, 0x00},	/* Register 32 */
//				{ 33, 0x05},	/* Register 33 */
//				{ 34, 0x00},	/* Register 34 */
//				{ 35, 0x00},	/* Register 35 */
//				{ 36, 0x05},	/* Register 36 */
//				{ 40, 0x80},	/* Register 40 */
//				{ 41, 0xA3},	/* Register 41 */
//				{ 42, 0xE5},	/* Register 42 */
//				{ 43, 0x00},	/* Register 43 */
//				{ 44, 0x06},	/* Register 44 */
//				{ 45, 0x11},	/* Register 45 */
//				{ 46, 0x00},	/* Register 46 */
//				{ 47, 0x1B},	/* Register 47 */
//				{ 48, 0xBF},	/* Register 48 */
//				{ 55, 0x00},	/* Register 55 */
//				{131, 0x1F},	/* Register 131 */
//				{132, 0x02},	/* Register 132 */
//				{137, 0x01},	/* Register 137 */
//				{138, 0x0F},	/* Register 138 */
//				{139, 0xFF},	/* Register 139 */
//				{142, 0x00},	/* Register 142 */
//				{143, 0x00},	/* Register 143 */
//				{136, 0x40}		/* Register 136 */

};



SI324Info InitTable_RTM[] = {
		//		100MHz free run
				{  0, 0x54},	/* Register 0 */
				{  1, 0xE1},	/* Register 1 */
				{  2, 0x82},	/* Register 2 */
				{  3, 0x55},	/* Register 3 */
				{  4, 0x92},	/* Register 4 */
				{  5, 0xAD},	/* Register 5 */
				{  6, 0x2D},	/* Register 6 */
				{  7, 0x2A},	/* Register 7 */
				{  8, 0x00},	/* Register 8 */
				{  9, 0xC0},	/* Register 9 */
				{ 10, 0x00},	/* Register 10 */
				{ 11, 0x40},	/* Register 11 */
				{ 19, 0x29},	/* Register 19 */
				{ 20, 0x3E},	/* Register 20 */
				{ 21, 0xFE},	/* Register 21 */
				{ 22, 0xDF},	/* Register 22 */
				{ 23, 0x1F},	/* Register 23 */
				{ 24, 0x3F},	/* Register 24 */
				{ 25, 0x60},	/* Register 25 */
				{ 31, 0x00},	/* Register 31 */
				{ 32, 0x00},	/* Register 32 */
				{ 33, 0x05},	/* Register 33 */
				{ 34, 0x00},	/* Register 34 */
				{ 35, 0x00},	/* Register 35 */
				{ 36, 0x05},	/* Register 36 */
				{ 40, 0x60},	/* Register 40 */
				{ 41, 0x0A},	/* Register 41 */
				{ 42, 0xA9},	/* Register 42 */
				{ 43, 0x00},	/* Register 43 */
				{ 44, 0x00},	/* Register 44 */
				{ 45, 0x5A},	/* Register 45 */
				{ 46, 0x00},	/* Register 46 */
				{ 47, 0x01},	/* Register 47 */
				{ 48, 0x9F},	/* Register 48 */
				{ 55, 0x00},	/* Register 55 */
				{131, 0x1F},	/* Register 131 */
				{132, 0x02},	/* Register 132 */
				{137, 0x01},	/* Register 137 */
				{138, 0x0F},	/* Register 138 */
				{139, 0xFF},	/* Register 139 */
				{142, 0x00},	/* Register 142 */
				{143, 0x00},	/* Register 143 */
				{136, 0x40}		/* Register 136 */
};



