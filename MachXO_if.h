#pragma once

#include "ctrl_i2c.h"
#include <vector>
#include <fstream>


class MachXO
{
public:
	// Erase Flag Enumeration
	enum {
		MACHXO_ERASE_SRAM = (1 << 16),
		MACHXO_ERASE_FEATURE_ROW = (1 << 17),
		MACHXO_ERASE_CONFIG_FLASH = (1 << 18),
		MACHXO_ERASE_UFM = (1 << 19),
	};

	// Feature bit pos
	enum {
		FB_MASTER_SPI_PERSISTENCE		= 1 << 3,
		FB_I2C_PORT_NON_PERSISTENCE		= 1 << 2,
		FB_SLAVE_SPI_NON_PERSISTENCE	= 1 << 1,
		FB_JTAG_PORT_NON_PERSISTENCE	= 1 << 0,
		FB_DONE_PERSISTENCE				= 1 << 15,
		FB_INITN_NON_PERSISTENCE		= 1 << 14,
		FB_PROGRAMN_NON_PERSISTENCE		= 1 << 13,
	};


public:
	MachXO( uint8_t i2c_addr = 0x40 )
		: m_io( i2c_addr )
	{
	}

	uint32_t    readDeviceID()
	{
		uint32_t    read_data = 0;

		m_io.write_and_read({ 0xE0, 0x00, 0x00, 0x00 }, (uint8_t*)&read_data, sizeof(read_data) );

		return  read_data;
	};

	uint32_t    readUserCode()
	{
		uint32_t    read_data = 0;

		m_io.write_and_read({ 0xC0, 0x00, 0x00, 0x00 }, (uint8_t*)&read_data, sizeof(read_data));

		return  read_data;
	};

	uint32_t    readStatus()
	{
		uint32_t    read_data = 0;

		m_io.write_and_read({ 0x3C, 0x00, 0x00, 0x00 },(uint8_t*)&read_data, sizeof(read_data));

		return  read_data;
	};
 

	uint16_t readFeatureBits()
	{
		uint16_t    read_data = 0;

		m_io.write_and_read({ 0xFB, 0x00, 0x00, 0x00 },(uint8_t*)&read_data, sizeof(read_data));
		return  read_data;
	}; 

	bool writeFeatureBits( uint16_t value )
	{
		uint16_t    read_data = 0;
		return	6 == m_io.write({ 0xF8, 0x00, 0x00, 0x00, (uint8_t)value, (uint8_t)(value >> 8) });
	};

	uint64_t readFeatureRow()
	{
		uint64_t    read_data = 0;

		m_io.write_and_read({ 0xE7, 0x00, 0x00, 0x00 },(uint8_t*)&read_data, sizeof(read_data));
		return  read_data;
	}

	bool	writeFeatureRow(uint64_t value = 0x0000000000100000 )
	{
		uint8_t	data[12] = { 0 };
		
		data[0] = 0xE4;
		*((uint64_t*)&data[4]) = value;

		return 12 == m_io.write(data, sizeof(data));
	}

	uint8_t readOTPFuses()
	{
		uint8_t    read_data = 0;

		m_io.write_and_read({ 0xFA, 0x00, 0x00, 0x00 },(uint8_t*)&read_data, sizeof(read_data));
		return  read_data;
	}

	std::vector<uint8_t> readFlash()
	{
		std::vector<uint8_t>    read_data(16, 0);

		m_io.write_and_read({ 0x73, 0x00, 0x00, 0x00 },read_data.data(), read_data.size());
		return  read_data;
	}

	std::vector<uint8_t> readUFM()
	{
		std::vector<uint8_t>    read_data(16, 0);

		m_io.write_and_read({ 0xCA, 0x00, 0x00, 0x00 }, read_data.data(), read_data.size());
		return  read_data;
	}

	bool    eraseUFM()
	{
		return 4 == m_io.write({ 0xCB, 0x00, 0x00, 0x00 });
	}

	bool    erase(uint32_t flags)
	{
		return 4 == m_io.write({ 0x0E, (uint8_t)(0x0F & (flags >> 16)), 0x00, 0x00 });
	}

	bool    enableConfigTransparent()
	{
		return 3 == m_io.write({ 0x74, 0x08, 0x00 });
	}

	bool    enableConfigOffline()
	{
		return 3 == m_io.write({ 0xC6, 0x08, 0x00 });
	}

	bool    isBusy()
	{
		uint8_t read_data   = 0;
		m_io.write_and_read({ 0xF0, 0x00, 0x00, 0x00 },(uint8_t*)&read_data, sizeof(read_data));
		return ((read_data & 0x80) ? 1 : 0);
	}

	uint32_t waitBusy()
	{
		uint32_t waitCnt = 0;
		while (isBusy()) {
			waitCnt += 1;
		}
		return (waitCnt);
	}

	bool    resetConfigAddress()
	{
		return  4 == m_io.write({ 0x46, 0x00, 0x00, 0x00 });
	}

	uint32_t resetUFMAddress()
	{
		return  4 == m_io.write({ 0x47, 0x00, 0x00, 0x00 });
	}

	uint32_t setConfigAddress(uint32_t page)
	{
		return  8 == m_io.write({ 0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, (uint8_t)((page >> 8) & 0xFF),(uint8_t)((page) & 0xFF) });
	}

	uint32_t setUFMAddress(uint32_t page)
	{
		return  4 == m_io.write({ 0xB4, 0x00, 0x00, 0x00, 0x40, 0x00, (uint8_t)((page >> 8) & 0xFF),(uint8_t)((page) & 0xFF) });
	}

	uint32_t programPage(uint8_t* obuf)
	{
		m_io.write(obuf, 20);
		waitBusy(); // a 200us delay is also acceptable, should not be needed with I2C
		return 0;
	}

	uint32_t programDone()
	{
		return  4 == m_io.write({ 0x5E, 0x00, 0x00, 0x00 });
	}

	uint32_t refresh()
	{
		return  3 == m_io.write({ 0x79, 0x00, 0x00 });
	}

	uint32_t wakeup()
	{
		return  4 == m_io.write({ 0xFF, 0xFF, 0xFF, 0xFF });
	}

	inline bool isHexadecimalDigit(char c)
	{
		return  (('0' <= c) && (c <= '9')) ||
				(('A' <= c) && (c <= 'F')) ||
				(('a' <= c) && (c <= 'f'));

	}

	uint32_t    loadHex(std::ifstream &ifs )
	{
		int byteCnt = 0;
		int pageCnt = 0;
		char nextChr;
		char hexByteStr[3] = { 0, 0, 0 };
		uint8_t pageBuf[20];
		pageBuf[0] = 0x70;
		pageBuf[1] = 0x00;
		pageBuf[2] = 0x00;
		pageBuf[3] = 0x01;
		resetConfigAddress();

		while ( !ifs.eof() )
		{
			nextChr = ifs.get();
			if (!isHexadecimalDigit(nextChr)) {
				byteCnt = 0;
			}
			else {
				if (byteCnt > 15) {
					printf( "[ERROR] loadHex, too many hex digits\n");
				}
				else {
					if (!ifs.eof()) {
						hexByteStr[0] = nextChr;
						hexByteStr[1] = ifs.get();
						pageBuf[byteCnt + 4] = strtoul(hexByteStr, NULL, 16);
						byteCnt += 1;
						if (byteCnt == 16) {
							programPage(pageBuf);
							pageCnt += 1;
							printf( "Loading HEX %d pages written\r", pageCnt);
						}
					}
					else {
						printf("\n[ERROR] loadHex, uneven number of hex digits");
					}
				}
			}
		}

		printf("Loading HEX %d pages written\n", pageCnt );
		if (byteCnt == 16) {
			byteCnt = 0;
		}

		printf("Loading HEX %d bytes left over\n", byteCnt);
		return (byteCnt);
	}


protected:
	ctrl_i2c   m_io;
};

