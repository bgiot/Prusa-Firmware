#include "uni_avr_rpi.h"

#ifdef PAT9125

#include "pat9125.h"

#ifdef PAT9125_SWSPI
#include "swspi.h"
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
#include "swi2c.h"
#endif //PAT9125_SWI2C


unsigned char pat9125_PID1 = 0;
unsigned char pat9125_PID2 = 0;
int pat9125_x = 0;
int pat9125_y = 0;
int pat9125_b = 0;

int pat9125_init(unsigned char xres, unsigned char yres)
{
#ifdef PAT9125_SWSPI
	swspi_init();
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
	swi2c_init(PAT9125_SWI2C_SDA, PAT9125_SWI2C_SCL, PAT9125_SWI2C_CFG);
#endif //PAT9125_SWI2C
	pat9125_PID1 = pat9125_rd_reg(PAT9125_PID1);
	pat9125_PID2 = pat9125_rd_reg(PAT9125_PID2);
	if ((pat9125_PID1 != 0x31) || (pat9125_PID2 != 0x91))
	{
		return 0;
	}
    pat9125_wr_reg(PAT9125_RES_X, xres);
    pat9125_wr_reg(PAT9125_RES_Y, yres);
	return 1;
}

int pat9125_update()
{
	if ((pat9125_PID1 == 0x31) && (pat9125_PID2 == 0x91))
	{
		unsigned char ucMotion = pat9125_rd_reg(PAT9125_MOTION);
		pat9125_b = pat9125_rd_reg(PAT9125_FRAME);
		if (ucMotion & 0x80)
		{
			unsigned char ucXL = pat9125_rd_reg(PAT9125_DELTA_XL);
			unsigned char ucYL = pat9125_rd_reg(PAT9125_DELTA_YL);
			unsigned char ucXYH = pat9125_rd_reg(PAT9125_DELTA_XYH);
			int iDX = ucXL | ((ucXYH << 4) & 0xf00);
			int iDY = ucYL | ((ucXYH << 8) & 0xf00);
			if (iDX & 0x800) iDX -= 4096;
			if (iDY & 0x800) iDY -= 4096;
			pat9125_x += iDX;
			pat9125_y += iDY;
			return 1;
		}
	}
	return 0;
}

unsigned char pat9125_rd_reg(unsigned char addr)
{
	unsigned char data = 0;
#ifdef PAT9125_SWSPI
	swspi_start();
	swspi_tx(addr & 0x7f);
	data = swspi_rx();
	swspi_stop();
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
	int iret = swi2c_readByte_A8(PAT9125_I2C_ADDR, addr, &data);
#endif //PAT9125_SWI2C
	return data;
}

void pat9125_wr_reg(unsigned char addr, unsigned char data)
{
#ifdef PAT9125_SWSPI
	swspi_start();
	swspi_tx(addr | 0x80);
	swspi_tx(data);
	swspi_stop();
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
	int iret = swi2c_writeByte_A8(PAT9125_I2C_ADDR, addr, &data);
#endif //PAT9125_SWI2C
}

#endif //PAT9125