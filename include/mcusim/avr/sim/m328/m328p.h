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
#ifndef MSIM_AVR_M328P_H_
#define MSIM_AVR_M328P_H_ 1

/* Include headers specific to the ATMega328P */
#define _SFR_ASM_COMPAT 1
#define __AVR_ATmega328P__ 1

#include <stdio.h>
#include <stdint.h>

#include "mcusim/avr/io.h"
#include "mcusim/avr/sim/m328/m328_ioregs.h"
#include "mcusim/avr/sim/io_regs.h"
#include "mcusim/avr/sim/sim.h"
#include "mcusim/avr/sim/simcore.h"
#include "mcusim/avr/sim/private/io_macro.h"

int	MSIM_M328PSetFuse(struct MSIM_AVR *mcu, struct MSIM_AVRConf *cnf);
int	MSIM_M328PSetLock(struct MSIM_AVR *mcu, struct MSIM_AVRConf *cnf);
int	MSIM_M328PUpdate(struct MSIM_AVR *mcu, struct MSIM_AVRConf *cnf);

/* ATMega328P Fuse Low Byte */
enum MSIM_AVRFuseLowByte {
	AVR_FLB_CKSEL0,
	AVR_FLB_CKSEL1,
	AVR_FLB_CKSEL2,
	AVR_FLB_CKSEL3,
	AVR_FLB_SUT0,
	AVR_FLB_SUT1,
	AVR_FLB_CKOUT,
	AVR_FLB_CKDIV8
};
enum MSIM_AVRFuseHighByte {
	AVR_FHB_BOOTRST,
	AVR_FHB_BOOTSZ0,
	AVR_FHB_BOOTSZ1,
	AVR_FHB_EESAVE,
	AVR_FHB_WDTON,
	AVR_FHB_SPIEN,
	AVR_FHB_DWEN,
	AVR_FHB_RSTDISBL
};
enum MSIM_AVRFuseExtended {
	AVR_FEXT_BODLEVEL0,
	AVR_FEXT_BODLEVEL1,
	AVR_FEXT_BODLEVEL2,
};

const static struct MSIM_AVR ORIG_M328P = {
	.name = "ATmega328P",
	.signature = { SIGNATURE_0, SIGNATURE_1, SIGNATURE_2 },
	.pc = 0x0000,
	.pc_bits = 14,
	.freq = 1000000,
	.clk_source = AVR_INT_CAL_RC_CLK,
	.lockbits = 0x3F,
	.regs_num = 32,
	.ioregs_num = 224,
	.xmega = 0,
	.reduced_core = 0,
	.spm_pagesize = SPM_PAGESIZE,
	.flashstart = 0x0000,
	.flashend = FLASHEND,
	.ramstart = RAMSTART,
	.ramend = RAMEND,
	.ramsize = 2048,
	.e2start = 0x0000,
	.e2end = E2END,
	.e2size = 1024,
	.e2pagesize = E2PAGESIZE,
	.sfr_off = __SFR_OFFSET,
	.set_fusef = MSIM_M328PSetFuse,
	.set_lockf = MSIM_M328PSetLock,
	.tick_perf = MSIM_M328PUpdate,
	.fuse = { LFUSE_DEFAULT, HFUSE_DEFAULT, 0xFF },
	.bls = {
		.start = 0x7000,
		.end = 0x7FFF,
		.size = 4096,
	},
	.intr = {
		.reset_pc = 0x0000,
		.ivt = 0x0004,
	},
	.ioports = {
		/* ----------------------- Port B -------------------------- */
		[0] = {
			.port = IOBYTE(PORTB),
			.ddr = IOBYTE(DDRB),
			.pin = IOBYTE(PINB)
		},
		/* ----------------------- Port C -------------------------- */
		[1] = {
			.port = IOBYTE(PORTC),
			.ddr = IOBYTE(DDRC),
			.pin = IOBYTE(PINC)
		},
		/* ----------------------- Port D -------------------------- */
		[2] = {
			.port = IOBYTE(PORTD),
			.ddr = IOBYTE(DDRD),
			.pin = IOBYTE(PIND)
		},
	},
	.timers = {
		[0] = {
			/* ---------------- Basic config ------------------- */
			.tcnt = { IOBYTE(TCNT0) },
			.disabled = IOBIT(PRR, PRTIM0),
			.size = 8,
			/* ------------- Clock select config --------------- */
			.cs = {
				IOBIT(TCCR0B, CS00), IOBIT(TCCR0B, CS01),
				IOBIT(TCCR0B, CS02)
			},
			.cs_div = { 0, 0, 3, 6, 8, 10 }, /* Power of 2 */
			/* ------- Waveform generation mode config --------- */
			.wgm = {
				IOBIT(TCCR0A, WGM00), IOBIT(TCCR0A, WGM01),
				IOBIT(TCCR0B, WGM02),
			},
			.wgm_op = {
				[0] = {
					.kind = WGM_NORMAL,
					.size = 8,
					.top = 0xFF,
					.updocr_at = UPD_ATIMMEDIATE,
					.settov_at = UPD_ATMAX,
				},
				[1] = {
					.kind = WGM_PCPWM,
					.size = 8,
					.top = 0xFF,
					.updocr_at = UPD_ATTOP,
					.settov_at = UPD_ATBOTTOM,
				},
				[2] = {
					.kind = WGM_CTC,
					.rtop = { IOBYTE(OCR0A) },
					.updocr_at = UPD_ATIMMEDIATE,
					.settov_at = UPD_ATMAX,
				},
				[3] = {
					.kind = WGM_FASTPWM,
					.size = 8,
					.top = 0xFF,
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATMAX,
				},
				[4] = {
					.kind = WGM_NONE,
				},
				[5] = {
					.kind = WGM_PCPWM,
					.rtop = { IOBYTE(OCR0A) },
					.updocr_at = UPD_ATTOP,
					.settov_at = UPD_ATBOTTOM,
				},
				[6] = {
					.kind = WGM_NONE,
				},
				[7] = {
					.kind = WGM_FASTPWM,
					.rtop = { IOBYTE(OCR0A) },
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATTOP,
				},
			},
			/* ------------ Input capture config --------------- */
			.icr = IONOBITA(),
			.icp = IONOBIT(),
			.ices = IONOBITA(),
			/* ------------- Interrupts config ----------------- */
			.iv_ovf = {
				.enable = IOBIT(TIMSK0, TOIE0),
				.raised = IOBIT(TIFR0, TOV0),
				.vector = TIMER0_OVF_vect_num
			},
			.iv_ic = NOINTV(),
			/* ----------- Output compare config --------------- */
			.comp = {
				[0] = {
					.ocr = { IOBYTE(OCR0A) },
					.pin = IOBIT(PORTD, PD6),
					.ddp = IOBIT(DDRD, PD6),
					.com = IOBITS(TCCR0A, COM0A0, 0x3, 2),
					.com_op = {
						[0] = { /* WGM_NORMAL */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[1] = { /* WGM_PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[2] = { /* WGM_CTC */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[3] = { /* WGM_FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
						[5] = { /* WGM_PCPWM */
							COM_DISC,
							COM_TGONCM,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[7] = { /* WGM_FASTPWM */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
					},
					.iv = {
						.enable = IOBIT(TIMSK0, OCIE0A),
						.raised = IOBIT(TIFR0, OCF0A),
						.vector = TIMER0_COMPA_vect_num
					},
				},
				[1] = {
					.ocr = { IOBYTE(OCR0B) },
					.pin = IOBIT(PORTD, PD5),
					.ddp = IOBIT(DDRD, PD5),
					.com = IOBITS(TCCR0A, COM0B0, 0x3, 2),
					.com_op = {
						[0] = { /* WGM_NORMAL */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[1] = { /* WGM_PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[2] = { /* WGM_CTC */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[3] = { /* WGM_FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
						[5] = { /* WGM_PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[7] = { /* WGM_FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
					},
					.iv = {
						.enable = IOBIT(TIMSK0, OCIE0B),
						.raised = IOBIT(TIFR0, OCF0B),
						.vector = TIMER0_COMPB_vect_num
					},
				},
			},
		},
		[1] = {
			/* ---------------- Basic config ------------------- */
			.tcnt = { IOBYTE(TCNT1L), IOBYTE(TCNT1H) },
			.disabled = IOBIT(PRR, PRTIM1),
			.size = 16,
			/* ------------- Clock select config --------------- */
			.cs = {
				IOBIT(TCCR1B, CS10), IOBIT(TCCR1B, CS11),
				IOBIT(TCCR1B, CS12)
			},
			.cs_div = { 0, 0, 3, 6, 8, 10 }, /* Power of 2 */
			/* ------- Waveform generation mode config --------- */
			.wgm = {
				IOBIT(TCCR1A, WGM10), IOBIT(TCCR1A, WGM11),
				IOBIT(TCCR1B, WGM12), IOBIT(TCCR1B, WGM13)
			},
			.wgm_op = {
				[0] = {
					.kind = WGM_NORMAL,
					.size = 16,
					.top = 0xFFFF,
					.updocr_at = UPD_ATIMMEDIATE,
					.settov_at = UPD_ATMAX,
				},
				[1] = {
					.kind = WGM_PCPWM,
					.size = 8,
					.top = 0x00FF,
					.updocr_at = UPD_ATTOP,
					.settov_at = UPD_ATBOTTOM,
				},
				[2] = {
					.kind = WGM_PCPWM,
					.size = 9,
					.top = 0x01FF,
					.updocr_at = UPD_ATTOP,
					.settov_at = UPD_ATBOTTOM,
				},
				[3] = {
					.kind = WGM_PCPWM,
					.size = 10,
					.top = 0x03FF,
					.updocr_at = UPD_ATTOP,
					.settov_at = UPD_ATBOTTOM,
				},
				[4] = {
					.kind = WGM_CTC,
					.rtop = {
						IOBYTE(OCR1AL),
						IOBYTE(OCR1AH),
					},
					.updocr_at = UPD_ATIMMEDIATE,
					.settov_at = UPD_ATMAX,
				},
				[5] = {
					.kind = WGM_FASTPWM,
					.size = 8,
					.top = 0x00FF,
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATTOP,
				},
				[6] = {
					.kind = WGM_FASTPWM,
					.size = 9,
					.top = 0x01FF,
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATTOP,
				},
				[7] = {
					.kind = WGM_FASTPWM,
					.size = 10,
					.top = 0x03FF,
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATTOP,
				},
				[8] = {
					.kind = WGM_PFCPWM,
					.rtop = {
						IOBYTE(ICR1L),
						IOBYTE(ICR1H),
					},
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATBOTTOM,
				},
				[9] = {
					.kind = WGM_PFCPWM,
					.rtop = {
						IOBYTE(OCR1AL),
						IOBYTE(OCR1AH),
					},
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATBOTTOM,
				},
				[10] = {
					.kind = WGM_PCPWM,
					.rtop = {
						IOBYTE(ICR1L),
						IOBYTE(ICR1H),
					},
					.updocr_at = UPD_ATTOP,
					.settov_at = UPD_ATBOTTOM,
				},
				[11] = {
					.kind = WGM_PCPWM,
					.rtop = {
						IOBYTE(OCR1AL),
						IOBYTE(OCR1AH),
					},
					.updocr_at = UPD_ATTOP,
					.settov_at = UPD_ATBOTTOM,
				},
				[12] = {
					.kind = WGM_CTC,
					.rtop = {
						IOBYTE(ICR1L),
						IOBYTE(ICR1H),
					},
					.updocr_at = UPD_ATIMMEDIATE,
					.settov_at = UPD_ATMAX,
				},
				[13] = {
					.kind = WGM_NONE,
				},
				[14] = {
					.kind = WGM_FASTPWM,
					.rtop = {
						IOBYTE(ICR1L),
						IOBYTE(ICR1H),
					},
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATTOP,
				},
				[15] = {
					.kind = WGM_FASTPWM,
					.rtop = {
						IOBYTE(OCR1AL),
						IOBYTE(OCR1AH),
					},
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATTOP,
				},
			},
			/* ------------ Input capture config --------------- */
			.icr = { IOBYTE(ICR1L), IOBYTE(ICR1H) },
			.icp = IOBIT(PORTB, PB0),
			.ices = { IOBIT(TCCR1B, ICES1) },
			/* ------------- Interrupts config ----------------- */
			.iv_ovf = {
				.enable = IOBIT(TIMSK1, TOIE1),
				.raised = IOBIT(TIFR1, TOV1),
				.vector = TIMER1_OVF_vect_num
			},
			.iv_ic = {
				.enable = IOBIT(TIMSK1, ICIE1),
				.raised = IOBIT(TIFR1, ICF1),
				.vector = TIMER1_CAPT_vect_num
			},
			/* ----------- Output compare config --------------- */
			.comp = {
				[0] = {
					.ocr = {
						IOBYTE(OCR1AL), IOBYTE(OCR1AH)
					},
					.pin = IOBIT(PORTB, PB1),
					.ddp = IOBIT(DDRB, PB1),
					.com = IOBITS(TCCR1A, COM1A0, 0x3, 2),
					.com_op = {
						[0] = { /* Normal mode */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[1] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[2] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[3] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[4] = { /* CTC */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[5] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
						[6] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
						[7] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
						[8] = { /* PFCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[9] = { /* PFCPWM */
							COM_DISC,
							COM_TGONCM,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[10] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[11] = { /* PCPWM */
							COM_DISC,
							COM_TGONCM,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[12] = { /* CTC */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[14] = { /* FASTPWM */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
						[15] = { /* FASTPWM */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
					},
					.iv = {
						.enable = IOBIT(TIMSK1, OCIE1A),
						.raised = IOBIT(TIFR1, OCF1A),
						.vector = TIMER1_COMPA_vect_num
					},
				},
				[1] = {
					.ocr = {
						IOBYTE(OCR1BL), IOBYTE(OCR1BH)
					},
					.pin = IOBIT(PORTB, PB2),
					.ddp = IOBIT(DDRB, PB2),
					.com = IOBITS(TCCR1A, COM1B0, 0x3, 2),
					.com_op = {
						[0] = { /* Normal mode */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[1] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[2] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[3] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[4] = { /* CTC */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[5] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
						[6] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
						[7] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
						[8] = { /* PFCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[9] = { /* PFCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[10] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[11] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[12] = { /* CTC */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[14] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
						[15] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONCM_STATBOT,
							COM_STONCM_CLATBOT,
						},
					},
					.iv = {
						.enable = IOBIT(TIMSK1, OCIE1B),
						.raised = IOBIT(TIFR1, OCF1B),
						.vector = TIMER1_COMPB_vect_num
					},
				}
			},
		},
		[2] = {
			/* ---------------- Basic config ------------------- */
			.tcnt = { IOBYTE(TCNT2) },
			.disabled = IOBIT(PRR, PRTIM2),
			.size = 8,
			/* ------------- Clock select config --------------- */
			.cs = {
				IOBIT(TCCR2B, CS20), IOBIT(TCCR2B, CS21),
				IOBIT(TCCR2B, CS22)
			},
			.cs_div = { 0, 0, 3, 5, 6, 7, 8, 10 }, /* Power of 2 */
			/* ------- Waveform generation mode config --------- */
			.wgm = {
				IOBIT(TCCR2A, WGM20), IOBIT(TCCR2A, WGM21),
				IOBIT(TCCR2B, WGM22)
			},
			.wgm_op = {
				[0] = {
					.kind = WGM_NORMAL,
					.size = 8,
					.top = 0xFF,
					.updocr_at = UPD_ATIMMEDIATE,
					.settov_at = UPD_ATMAX,
				},
				[1] = {
					.kind = WGM_PCPWM,
					.size = 8,
					.top = 0xFF,
					.updocr_at = UPD_ATTOP,
					.settov_at = UPD_ATBOTTOM,
				},
				[2] = {
					.kind = WGM_CTC,
					.rtop = { IOBYTE(OCR2A) },
					.updocr_at = UPD_ATIMMEDIATE,
					.settov_at = UPD_ATMAX,
				},
				[3] = {
					.kind = WGM_FASTPWM,
					.size = 8,
					.top = 0xFF,
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATMAX,
				},
				[4] = {
					.kind = WGM_NONE,
				},
				[5] = {
					.kind = WGM_PCPWM,
					.rtop = { IOBYTE(OCR2A) },
					.updocr_at = UPD_ATTOP,
					.settov_at = UPD_ATBOTTOM,
				},
				[6] = {
					.kind = WGM_NONE,
				},
				[7] = {
					.kind = WGM_FASTPWM,
					.rtop = { IOBYTE(OCR2A) },
					.updocr_at = UPD_ATBOTTOM,
					.settov_at = UPD_ATTOP,
				},
			},
			/* ------------ Input capture config --------------- */
			.icr = IONOBITA(),
			.icp = IONOBIT(),
			.ices = IONOBITA(),
			/* ------------- Interrupts config ----------------- */
			.iv_ovf = {
				.enable = IOBIT(TIMSK2, TOIE2),
				.raised = IOBIT(TIFR2, TOV2),
				.vector = TIMER2_OVF_vect_num
			},
			.iv_ic = NOINTV(),
			/* ----------- Output compare config --------------- */
			.comp = {
				[0] = {
					.ocr = { IOBYTE(OCR2A) },
					.pin = IOBIT(PORTB, PB3),
					.ddp = IOBIT(DDRB, PB3),
					.com = IOBITS(TCCR2A, COM2A0, 0x3, 2),
					.com_op = {
						[0] = { /* Normal mode */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[1] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[2] = { /* CTC */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[3] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[5] = { /* PCPWM */
							COM_DISC,
							COM_TGONCM,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[7] = { /* FASTPWM */
							COM_DISC,
							COM_TGONCM,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
					},
					.iv = {
						.enable = IOBIT(TIMSK2, OCIE2A),
						.raised = IOBIT(TIFR2, OCF2A),
						.vector = TIMER2_COMPA_vect_num
					},
				},
				[1] = {
					.ocr = { IOBYTE(OCR2B) },
					.pin = IOBIT(PORTD, PD3),
					.ddp = IOBIT(DDRD, PD3),
					.com = IOBITS(TCCR2A, COM2B0, 0x3, 2),
					.com_op = {
						[0] = { /* Normal mode */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[1] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[2] = { /* CTC */
							COM_DISC,
							COM_TGONCM,
							COM_CLONCM,
							COM_STONCM,
						},
						[3] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[5] = { /* PCPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
						[7] = { /* FASTPWM */
							COM_DISC,
							COM_DISC,
							COM_CLONUP_STONDOWN,
							COM_STONUP_CLONDOWN,
						},
					},
					.iv = {
						.enable = IOBIT(TIMSK2, OCIE2B),
						.raised = IOBIT(TIFR2, OCF2B),
						.vector = TIMER2_COMPB_vect_num
					},
				},
			},
		},
	},
	.wdt = {
		.wdton = FBIT(1, WDTON),
		.wde = IOBIT(WDTCSR, WDE),
		.wdie = IOBIT(WDTCSR, WDIE),
		.ce = IOBIT(WDTCSR, WDCE),
		.oscf = 128000,
		.wdp = {
			IOBIT(WDTCSR, WDP0), IOBIT(WDTCSR, WDP1),
			IOBIT(WDTCSR, WDP2), IOBIT(WDTCSR, WDP3)
		},
		.wdp_op = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 },
		.iv_tout = {
			.enable = IOBIT(WDTCSR, WDIE),
			.raised = IOBIT(WDTCSR, WDIF),
			.vector = WDT_vect_num
		},
		.iv_sysr = {
			.enable = IONOBIT(),
			.raised = IONOBIT(),
			.vector = 0 /* Reset vector */
		},
	}
};

#endif /* MSIM_AVR_M328P_H_ */
