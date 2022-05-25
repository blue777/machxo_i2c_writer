#ifndef __CTRL_I2C_H_INCLUDED__
#define __CTRL_I2C_H_INCLUDED__

#include <stdint.h>
#include <cstdint>
#include <initializer_list>

class ctrl_io_if
{
public:
	virtual ~ctrl_io_if() {};
	virtual	int32_t	read( uint8_t* data, size_t size) = 0;
	virtual	int32_t write(const uint8_t* data, size_t size) = 0;

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
		m_handle = Mcp2221_OpenByIndex(MCP2221_USB_VID, MCP2221_USB_PID, 0);
		Mcp2221_SetSpeed(m_handle, 400000);
	}

	~ctrl_i2c()
	{
		Mcp2221_Close(m_handle);
	}

	int32_t	write( const uint8_t* data, size_t size )
	{
		return	0 == Mcp2221_I2cWrite(
			m_handle,
			(unsigned int)size,
			m_addr,
			1,	// 7bit mode
			(unsigned char *)data) ? (int32_t)size : 0;
	}

	int32_t  read(uint8_t* data, size_t size )
	{
		return 0 == Mcp2221_I2cRead(
			m_handle,
			(unsigned int)size,
			m_addr,
			1,	// 7bit mode
			(unsigned char *)data) ? (int32_t)size : 0;
	}

	int32_t write(std::initializer_list<const uint8_t> data)
	{
		return	write(data.begin(), data.size());
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


class ctrl_i2c : public ctrl_io_if
{
public:
    ctrl_i2c(uint8_t addr, const char * dev = "/dev/i2c-0" )
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
	return		::read( m_i2c, data, size );
    }


	int32_t write(std::initializer_list<const uint8_t> data)
	{
		return	write(data.begin(), data.size());
	}

private:
    int     m_i2c;
};

#endif


#endif
