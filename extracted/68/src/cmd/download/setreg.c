/* setreg.c - setreg */

#include "define.h"
#include "regdefs.h"

/*------------------------------------------------------------------------------
 * setreg  --  preload a register through the monitor
 *------------------------------------------------------------------------------
 */
setreg(reg, value, type)
int 	reg;
int	value;
int	type;
{
	int	creg;
	char	buf[32];

	creg = reg;
	switch (type) {
		case ADDRESS:
		case DATA:
			sprintf(buf, "%c%d\r", type, reg);
			sendmon(buf, WAIT);
			readuntil("? ", buf, 3);
			sprintf(buf,"%x\r", value);
			sendmon(buf, WAIT);
			break;
	
		case SPECIAL:
			sendmon("R\r", WAIT);
			readuntil("? ", buf, 3);
			while (--creg) {
				sendmon("\r", WAIT);
				readuntil("? ", buf, 3);
			}
			sprintf(buf,"%x\r", value);
			sendmon(buf, WAIT);
			if (reg == 4) {
				readuntil(">", buf, 3);
				return;
			}
			break;

		case MEMORYWORD:
			sprintf(buf, "e %08x\r", reg);
			sendmon(buf, WAIT);
			sprintf(buf, "%04x\r", value);
			sendmon(buf, WAIT);
			break;

		case MEMORYBYTE:
			sprintf(buf, "o %08x\r", reg);
			sendmon(buf, WAIT);
			sprintf(buf, "%02x\r", value);
			sendmon(buf, WAIT);
			break;

		default:
			error("Bad register type %c", type);
	}
	readuntil("? ", buf, 3);
	sendmon("q\r", WAIT);
	readuntil(">",buf,3);
}
