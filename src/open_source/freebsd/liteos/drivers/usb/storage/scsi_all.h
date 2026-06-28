/*-
 * Largely written by Julian Elischer (julian@tfs.com)
 * for TRW Financial Systems.
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2025. All rights reserved.
 *
 * TRW Financial Systems, in accordance with their agreement with Carnegie
 * Mellon University, makes this software available to CMU to distribute
 * or use in any manner that they see fit as long as this message is kept with
 * the software. For this reason TFS also grants any other persons or
 * organisations permission to use or modify this software.
 *
 * TFS supplies this software to be publicly redistributed
 * on the understanding that TFS is not responsible for the correct
 * functioning of this software in any circumstances.
 *
 * Ported to run under 386BSD by Julian Elischer (julian@tfs.com) Sept 1992
 */

#ifndef __INCLUDE_SCSI_ALL_H
#define	__INCLUDE_SCSI_ALL_H

#define	SC_SCSI_1 0x01
#define	SC_SCSI_2 0x03

#define	SCSI_REV_2 0x02

/*
 * Opcodes
 */

#define	TEST_UNIT_READY		0x00
#define	REQUEST_SENSE		0x03
#define	READ_6			0x08
#define	WRITE_6			0x0A
#define	INQUIRY			0x12
#define	MODE_SELECT_6		0x15
#define	MODE_SENSE_6		0x1A
#define	START_STOP_UNIT		0x1B
#define	START_STOP		0x1B
#define	RESERVE      		0x16
#define	RELEASE      		0x17
#define	RECEIVE_DIAGNOSTIC	0x1C
#define	SEND_DIAGNOSTIC		0x1D
#define	PREVENT_ALLOW		0x1E
#define	READ_CAPACITY		0x25
#define	READ_10			0x28
#define	WRITE_10		0x2A
#define	POSITION_TO_ELEMENT	0x2B
#define	WRITE_VERIFY_10		0x2E
#define	VERIFY_10		0x2F
#define	SYNCHRONIZE_CACHE	0x35
#define	READ_DEFECT_DATA_10	0x37
#define	WRITE_BUFFER		0x3B
#define	READ_BUFFER			0x3C
#define	CHANGE_DEFINITION	0x40
#define	WRITE_SAME_10		0x41
#define	UNMAP			0x42
#define	LOG_SELECT		0x4C
#define	LOG_SENSE		0x4D
#define	MODE_SELECT_10		0x55
#define	RESERVE_10		0x56
#define	RELEASE_10		0x57
#define	MODE_SENSE_10		0x5A
#define	PERSISTENT_RES_IN	0x5E
#define	PERSISTENT_RES_OUT	0x5F
#define	EXTENDED_CDB		0x7E
#define	VARIABLE_LEN_CDB	0x7F
#define	EXTENDED_COPY		0x83
#define	RECEIVE_COPY_STATUS	0x84
#define	ATA_PASS_16		0x85
#define	READ_16			0x88
#define	COMPARE_AND_WRITE	0x89
#define	WRITE_16		0x8A
#define	READ_ATTRIBUTE		0x8C
#define	WRITE_ATTRIBUTE		0x8D
#define	WRITE_VERIFY_16		0x8E
#define	VERIFY_16		0x8F
#define	SYNCHRONIZE_CACHE_16	0x91
#define	WRITE_SAME_16		0x93
#define	READ_BUFFER_16		0x9B
#define	WRITE_ATOMIC_16		0x9C
#define	SERVICE_ACTION_IN	0x9E
#define	REPORT_LUNS		0xA0
#define	ATA_PASS_12		0xA1
#define	SECURITY_PROTOCOL_IN	0xA2
#define	MAINTENANCE_IN		0xA3
#define	MAINTENANCE_OUT		0xA4
#define	MOVE_MEDIUM     	0xA5
#define	READ_12			0xA8
#define	WRITE_12		0xAA
#define	WRITE_VERIFY_12		0xAE
#define	VERIFY_12		0xAF
#define	SECURITY_PROTOCOL_OUT	0xB5
#define	READ_ELEMENT_STATUS	0xB8
#define	READ_CD			0xBE

/* Maintenance In Service Action Codes */
#define	REPORT_IDENTIFYING_INFRMATION		0x05
#define	REPORT_TARGET_PORT_GROUPS		0x0A
#define	REPORT_ALIASES				0x0B
#define	REPORT_SUPPORTED_OPERATION_CODES	0x0C
#define	REPORT_SUPPORTED_TASK_MANAGEMENT_FUNCTIONS	0x0D
#define	REPORT_PRIORITY				0x0E
#define	REPORT_TIMESTAMP			0x0F
#define	MANAGEMENT_PROTOCOL_IN			0x10
#define GET_PHYSICAL_ELEMENT_STATUS		0x17
#define REMOVE_ELEMENT_AND_TRUNCATE		0x18
#define RESTORE_ELEMENTS_AND_REBUILD		0x19
/* Maintenance Out Service Action Codes */
#define	SET_IDENTIFY_INFORMATION		0x06
#define	SET_TARGET_PORT_GROUPS			0x0A
#define	CHANGE_ALIASES				0x0B
#define	SET_PRIORITY				0x0E
#define	SET_TIMESTAMP				0x0F
#define	MANAGEMENT_PROTOCOL_OUT			0x10

/*
 * Define dome bits that are in ALL (or a lot of) scsi commands
 */
#define	T_DIRECT	0x00
#define	T_SEQUENTIAL	0x01
#define	T_PRINTER	0x02
#define	T_PROCESSOR	0x03
#define	T_WORM		0x04
#define	T_CDROM		0x05
#define	T_SCANNER	0x06
#define	T_OPTICAL 	0x07
#define	T_CHANGER	0x08
#define	T_COMM		0x09
#define	T_ASC0		0x0a
#define	T_ASC1		0x0b
#define	T_STORARRAY	0x0c
#define	T_ENCLOSURE	0x0d
#define	T_RBC		0x0e
#define	T_OCRW		0x0f
#define	T_OSD		0x11
#define	T_ADC		0x12
#define	T_ZBC_HM	0x14
#define	T_NODEVICE	0x1f
#define	T_ANY		0xff	/* Used in Quirk table matches */

#define	T_REMOV		1
#define	T_FIXED		0

/*
 * This length is the initial inquiry length used by the probe code, as
 * well as the length necessary for scsi_print_inquiry() to function
 * correctly.  If either use requires a different length in the future,
 * the two values should be de-coupled.
 */
#define	SHORT_INQUIRY_LENGTH	36

struct scsi_test_unit_ready
{
	uint8_t opcode;
	uint8_t byte2;
	uint8_t unused[3];
	uint8_t control;
};

struct scsi_sense
{
	uint8_t opcode;
	uint8_t byte2;
	uint8_t unused[2];
	uint8_t length;
	uint8_t control;
};

struct scsi_inquiry
{
	uint8_t opcode;
	uint8_t byte2;
#define	SI_EVPD 	0x01
#define	SI_CMDDT	0x02
	uint8_t page_code;
	uint8_t length[2];
	uint8_t control;
};

struct scsi_sense_data
{
	uint8_t error_code;
	/*
	 * SPC-4 says that the maximum length of sense data is 252 bytes.
	 * So this structure is exactly 252 bytes log.
	 */
#define	SSD_FULL_SIZE 252
	uint8_t sense_buf[SSD_FULL_SIZE - 1];
	/*
	 * Followed by a separator and iSCSI initiator session ID
	 */
#define	SSD_MIN_SIZE 18
	/*
	 * Attributes obtained from SPC-4r36g (section 7.4.2.2) and
	 * SSC-4r03 (section 4.2.21).
	 */
#define	SSD_EXTRA_MAX 244
};

struct scsi_start_stop_unit
{
	uint8_t opcode;
	uint8_t byte2;
#define	SSS_IMMED		0x01
	uint8_t reserved[2];
	uint8_t how;
#define	SSS_START		0x01
#define	SSS_LOEJ		0x02
#define	SSS_PC_MASK		0xf0
#define	SSS_PC_START_VALID	0x00
#define	SSS_PC_ACTIVE		0x10
#define	SSS_PC_IDLE		0x20
#define	SSS_PC_STANDBY		0x30
#define	SSS_PC_LU_CONTROL	0x70
#define	SSS_PC_FORCE_IDLE_0	0xa0
#define	SSS_PC_FORCE_STANDBY_0	0xb0
	uint8_t control;
};

/*
 * Reference 'SCSI Commands Reference Manual' section 3.26.2 READ CAPACITY (16) parameter data, table 100.
 */
struct scsiresp_readcapacity16_s
{
	uint8_t lba[8];			/* 0-7: Returned logical block address (LBA) */
	uint8_t blklen[4];		/* 8-11: Logical block length (in bytes) */
	uint8_t prot;			/* 12: Bits 0: PROT_EN, Bits 1-3: P_TYPE, Bits 4-7: Reserved*/
	uint8_t reserved[19];	/* 13-31: Reserved */
};
#define SCSIRESP_READCAPACITY16_SIZEOF 32

/*
 * Reference 'SCSI Commands Reference Manual' section 3.22 READ (16) command, table 89.
 */
struct scsicmd_read16_s
{
	uint8_t opcode;		/* 0: 0x88 */
	uint8_t flags;		/* 1: See SCSICMD_READ16 FLAGS* */
	uint8_t lba[8];		/* 2-9: Logical Block Address (LBA) */
	uint8_t xfrlen[4];	/* 10-13: Transfer length (in contiguous logical blocks) */
	uint8_t groupno;	/* 14: Bits 7: Restricted for MMC-4; Bits 5-6: reserved; Bits 0-4: group number */
	uint8_t control;	/* 15: Control */
};
#define SCSICMD_READ16_SIZEOF 16

/*
 * Reference 'SCSI Commands Reference Manual' section 3.56 WRITE (16) command, table 158.
 */
struct scsicmd_write16_s
{
	uint8_t opcode;		/* 0: 0x8a */
	uint8_t flags;		/* 1: See SCSICMD_WRITE10FLAGS_* */
	uint8_t lba[8];		/* 2-9: Logical Block Address (LBA) */
	uint8_t xfrlen[4];	/* 10-13: Transfer length (in contiguous logical blocks) */
	uint8_t groupno;	/* 14: Bits 7: RESTRICTED FOR MMC-4; Bits 5-6: reserved; Bits 0-4: group number */
	uint8_t control;	/* 9: Control */
};
#define SCSICMD_WRITE16_SIZEOF 16

/*
 * Opcodes
 */
#define	VERIFIED			0x2f
#define	REZERO_UNIT			0x01
#define	FORMAT_UNIT			0x04
#define	REASSIGN_BLOCKS		0x07
#define	MODE_SELECT			0x15
#define	MODE_SENSE			0x1a
#define	READ_FORMAT_CAPACITIES	0x23
#define	WRITE_AND_VERIFY	0x2e
#define	READ_DEFECT_DATA_10	0x37
#define	SANITIZE			0x48
#define	READ_DEFECT_DATA_12	0xb7

#define SCSICMD_TESTUNITREADY8_SIZEOF 6
#define SCSI_GET_INQUIRY_PDT(pdt) ((pdt) & 0x1f)	/* get the Peripheral Device Type */
#define SCSI_GET_INQUIRY_RMB(rmb) ((rmb) >> 7)		/* get Removable Medium Bit */

#endif /*_SCSI_SCSI_ALL_H*/
