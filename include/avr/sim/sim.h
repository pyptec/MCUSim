/*
 * mcusim - Interactive simulator for microcontrollers.
 * Copyright (C) 2017 Dmitry Salychev <darkness.bsd@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MSIM_AVR_SIM_H_
#define MSIM_AVR_SIM_H_ 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t avr_flashaddr_t;

enum avr_state {
	AVR_RUNNING = INT16_MIN,
	AVR_STOPPED,
	AVR_SLEEPING,
};

enum init_state {
	INITIALIZED = INT16_MIN,
	NULL_MCU,
	ERROR,
};

/*
 * Instance of the AVR microcontroller.
 */
struct avr {
	char *name;			/* Name of the MCU */

	uint16_t spm_pagesize;		/* For devices with bootloader support,
					   the flash pagesize (in bytes) to be
					   used for SPM instruction. */

	uint16_t flashstart;		/* The first byte address in flash
					   program space. */
	uint16_t flashend;		/* The last byte address in flash
					   program space. */

	uint16_t ramstart;
	uint16_t ramend;
	uint32_t ramsize;

	uint16_t e2start;		/* The first EEPROM address */
	uint16_t e2end;			/* The last EEPROM address */
	uint16_t e2size;
	uint16_t e2pagesize;		/* The size of the EEPROM page */

	uint8_t lockbits;
	uint8_t fuse[6];

	enum avr_state state;
	uint32_t freq;			/* Frequency we're currently
					   working at. */

	avr_flashaddr_t pc;		/* Current program counter register */
	avr_flashaddr_t reset_pc;	/* This is a value used to jump to
					   at reset time. It allows support
					   for bootloaders. */

	uint8_t *sreg;			/* Points directly to SREG placed
					   in data section. */

	uint8_t *prog_mem;		/* Flash memory */
	uint8_t *data_mem;		/* General purpose registers,
					   IO registers and SRAM */
};

/* Public prototypes */

enum init_state m8a_init(struct avr *mcu);

/* END Public prototypes */

#ifdef __cplusplus
}
#endif

#endif /* MSIM_AVR_SIM_H_ */