/*
 * This file is part of MCUSim, an XSPICE library with microcontrollers.
 *
 * Copyright (C) 2017-2019 MCUSim Developers, see AUTHORS.txt for contributors.
 *
 * MCUSim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MCUSim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* Unit tests for AVR Instruction Set */
#include <inttypes.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "mcusim/mcusim.h"
#include "mcusim/config.h"
#include "mcusim/log.h"
#include "mcusim/avr/sim/private/macro.h"
#include "mcusim/avr/sim/private/io_macro.h"

#define TEST_PREF		"files/XlingFirmware_1"
#define CONF_FILE		TEST_PREF ".ft.conf"
#define GDB_RSP_PORT		12750
#define FRM_TEST		1
#define SREGR			(*mcu->sreg)

#define RESTORE_MCU() do {						\
	_ctl = _orig;							\
	_avr = _orig;							\
} while (0)

/* Structure to keep a PC value and a dump of the data memory. */
struct dm_checkpoint {
	uint32_t pc;
	char dump[1024];
};

static MSIM_AVR _orig;
static MSIM_AVR _ctl;
static MSIM_AVR _avr;
static MSIM_AVR *mcu = &_avr;
static MSIM_AVR *ctl = &_ctl;

static MSIM_CFG conf = {
	.firmware_test = FRM_TEST,
	.rsp_port = GDB_RSP_PORT,
};

/* Check points to stop and check data memory. */
static struct dm_checkpoint dm_checkpoints[] = {
	{ 0x0000, TEST_PREF ".dm_0_0000.hex" },
	{ 0x057C, TEST_PREF ".dm_1_57C.hex" },
	{ 0x057D, TEST_PREF ".dm_2_57D.hex" },
	{ 0x057E, TEST_PREF ".dm_3_57E.hex" },
	{ 0x057F, TEST_PREF ".dm_4_57F.hex" },
	{ 0x0580, TEST_PREF ".dm_5_580.hex" },
	{ 0x0581, TEST_PREF ".dm_6_581.hex" },
	{ 0x0582, TEST_PREF ".dm_7_582.hex" },
	{ 0x0585, TEST_PREF ".dm_8_585.hex" },
	{ 0x0586, TEST_PREF ".dm_9_586.hex" },
	{ 0x0589, TEST_PREF ".dm_10_589.hex" },
	{ 0x058C, TEST_PREF ".dm_11_58C.hex" },
	{ 0x058F, TEST_PREF ".dm_12_58F.hex" },
	{ 0x0592, TEST_PREF ".dm_13_592.hex" },
	{ 0x0593, TEST_PREF ".dm_14_593.hex" },
	{ 0x0594, TEST_PREF ".dm_15_594.hex" },
};
/* Index of the datamem check point */
static uint32_t dmcp = 0;

/*
 * Perform AVR simulation with occasional stops to compare actual
 * data memory with HEX dumps (obtained from the Atmel Studio 7 with
 * MemoryLogger plugin).
 */
static void
check_datamem(void **state)
{
	/* Compare the whole datamem (0x900 for m328p) */
	const size_t memsz = mcu->ramend + 1;

	/* Initialize data memory */
	if (dm_checkpoints[dmcp].pc == 0U) {
		snprintf(LOG, LOGSZ, "init datamem: step=%" PRIu32 ", pc=0x%"
		         PRIx32 ", size=0x%" PRIx64 " (%" PRIu64 ")",
		         dmcp, mcu->pc, memsz, memsz);
		MSIM_LOG_INFO(LOG);

		MSIM_AVR_LoadDataMem(mcu, dm_checkpoints[dmcp].dump);
		dmcp++;
	}

	do {
		int rc = MSIM_AVR_SimStep(mcu, FRM_TEST);

		/* Stop if there was an error during simulation step. */
		if (rc != 0) {
			MSIM_LOG_FATAL("simulation error, terminating...");
			assert_int_equal(rc, 0);
		}

		/* Skip checking if it's not a checkpoint */
		if (mcu->pc != dm_checkpoints[dmcp].pc) {
			continue;
		}

		/*
		 * Data memory can be uninitialized if we didn't have
		 * a dump file for PC == 0x0000.
		 */
		if (dmcp == 0U) {
			snprintf(LOG, LOGSZ, "init datamem: step=%" PRIu32
			         ", pc=0x%" PRIx32 ", size=0x%" PRIx64 " (%"
			         PRIu64 ")", dmcp, mcu->pc, memsz, memsz);
			MSIM_LOG_INFO(LOG);

			MSIM_AVR_LoadDataMem(mcu, dm_checkpoints[dmcp].dump);
			dmcp++;
			continue;
		}

		snprintf(LOG, LOGSZ, "comparing datamem: step=%" PRIu32
		         ", pc=0x%" PRIx32 ", size=0x%" PRIx64 " (%"
		         PRIu64 ")...", dmcp, mcu->pc, memsz, memsz);
		MSIM_LOG_INFO(LOG);

		/* Load DM from a file to the 'control' MCU */
		MSIM_AVR_LoadDataMem(ctl, dm_checkpoints[dmcp].dump);

		/*
		 * Compare data memories of the 'current' and
		 * 'control' MCUs.
		 */
		assert_memory_equal(mcu->dm, ctl->dm, memsz);
		MSIM_LOG_INFO("Done");

		dmcp++;
	} while (dmcp < ARRSZ(dm_checkpoints));
}

static int
restore_mcu(void **state)
{
	RESTORE_MCU();
	return 0;
}

int
main(void)
{
	int rc = 0;

	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(check_datamem, restore_mcu),
	};

	MSIM_CFG_PrintVersion();

	do {
		/* Debug output from MCUSim */
		MSIM_LOG_SetLevel(MSIM_LOG_LVLDEBUG);

		/* Read config file */
		rc = MSIM_CFG_Read(&conf, CONF_FILE);
		if (rc != 0) {
			break;
		}

		/* Force firmware test option */
		conf.firmware_test = 1;

		/* Initialize AVR MCU */
		rc = MSIM_AVR_Init(mcu, &conf);
		_orig = *mcu;
		_ctl = *mcu;

		/* Don't write any registers to VCD */
		mcu->vcd.regs[0].i = -1;

		/* Force running state */
		mcu->state = AVR_RUNNING;
	} while (0);

	return rc != 0 ? rc : cmocka_run_group_tests(tests, NULL, NULL);
}
