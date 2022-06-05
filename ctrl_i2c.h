#ifndef __CTRL_I2C_H_INCLUDED__
#define __CTRL_I2C_H_INCLUDED__


#ifndef I2CBUS
#define I2CBUS	0
#endif


#include <stdint.h>
#include <cstdint>
#include <initializer_list>

class ctrl_io_if
{
public:
	virtual ~ctrl_io_if() {};
	virtual	int32_t	read( uint8_t* data, size_t size) = 0;
	virtual	int32_t write(const uint8_t* data, size_t size) = 0;
	virtual	int32_t	write_and_read(const uint8_t* tx_data, size_t tx_size, uint8_t* rx_data, size_t rx_size) = 0;
};



#ifdef _WIN32


//#define MCP2221_LIB
#include <mcp2221_dll_um.h>
#pragma comment( lib, "mcp2221_dll_um_x64.lib" )

#define	MCP2221_USB_VID		0x04D8
#define	MCP2221_USB_PID		0x00DD

class ctrl_i2c : public ctrl_io_if
{
public:
	ctrl_i2c(uint8_t addr ) : m_addr(addr)
	{
		m_handle = Mcp2221_OpenByIndex(MCP2221_USB_VID, MCP2221_USB_PID, I2CBUS );
		Mcp2221_SetSpeed(m_handle, 400000);
	}

	~ctrl_i2c()
	{
		Mcp2221_Close(m_handle);
	}

	int32_t	write( const uint8_t* data, size_t size )
	{
		return	Mcp2221_I2cWrite(
			m_handle,
			(unsigned int)size,
			m_addr,
			1,	// 7bit mode
			(unsigned char *)data ) == 0 ? (int32_t)size : 0;
	}

	int32_t  read(uint8_t* data, size_t size )
	{
		return Mcp2221_I2cRead(
			m_handle,
			(unsigned int)size,
			m_addr,
			1,	// 7bit mode
			(unsigned char *)data ) == 0 ? (int32_t)size : 0;
	}

	int32_t	write_and_read(const uint8_t* tx_data, size_t tx_size, uint8_t* rx_data, size_t rx_size)
	{
		Mcp2221_I2cWriteNoStop(
			m_handle,
			(unsigned int)tx_size,
			m_addr,
			1,	// 7bit mode
			(unsigned char*)tx_data);

		return Mcp2221_I2cReadRestart(m_handle,
			(unsigned int)rx_size,
			m_addr,
			1,	// 7bit mode
			(unsigned char*)rx_data) == 0 ? rx_size : 0;
	}



	int32_t write(std::initializer_list<const uint8_t> data)
	{
		return	write(data.begin(), data.size());
	}

	int32_t write_and_read(std::initializer_list<const uint8_t> tx_data, uint8_t* rx_data, size_t rx_size )
	{
		return	write_and_read( tx_data.begin(), tx_data.size(), rx_data, rx_size);
	}

private:
	void*		m_handle;
	const uint8_t m_addr;
};

#else

#include <stdio.h>
#include <unistd.h>
#include <exception>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#define	TOSTR2(s)	#s
#define	TOSTR(s)	TOSTR2(s)

class ctrl_i2c : public ctrl_io_if
{
public:
    ctrl_i2c(uint8_t addr, const char * dev = "/dev/i2c-" TOSTR(I2CBUS) ) : m_addr(addr)
    {
        int     ret;

        m_i2c    = ::open( dev, O_RDWR );
        if( m_i2c < 0 )
        {
            printf("ERROR! ctrl_i2c() open( %s ) ret %d\n", dev, m_i2c );
            throw std::runtime_error("ERROR: ctrl_i2c(), open() failed.");
        }

        ret     = ::ioctl( m_i2c, I2C_SLAVE, addr );
        if( ret < 0 )
        {
            printf("ERROR! ctrl_i2c() ioctl ret %d\n", ret );
            throw std::runtime_error("ERROR: ctrl_i2c(), ioctl(I2C_SLAVE) failed.");
        }
    }


	int32_t	write( const uint8_t* data, size_t size )
	{
		return	::write( m_i2c, data, size );
	}

	int32_t  read(uint8_t* data, size_t size )
	{
		return	::read( m_i2c, data, size );
	}

	int32_t	write_and_read(const uint8_t* tx_data, size_t tx_size, uint8_t* rx_data, size_t rx_size)
	{
		struct i2c_rdwr_ioctl_data	data;
		struct i2c_msg msg[2];
		int	ret;

		data.nmsgs = 2;
		data.msgs = msg;

		data.msgs[0].addr = m_addr;
		data.msgs[0].len = tx_size;
		data.msgs[0].flags = 0;
		data.msgs[0].buf = (uint8_t*)tx_data;

		data.msgs[1].addr = m_addr;
		data.msgs[1].len = rx_size;
		data.msgs[1].flags = I2C_M_RD;
		data.msgs[1].buf = rx_data;

		ret = ::ioctl( m_i2c, I2C_RDWR, &data);
		
		return ret < 0 ? -1 : rx_size;
	}

	int32_t write(std::initializer_list<const uint8_t> data)
	{
		return	write(data.begin(), data.size());
	}

	int32_t write_and_read(std::initializer_list<const uint8_t> tx_data, uint8_t* rx_data, size_t rx_size)
	{
		return	write_and_read(tx_data.begin(), tx_data.size(), rx_data, rx_size);
	}

private:
	int		m_i2c;
	const uint8_t m_addr;
};

#endif


#endif
