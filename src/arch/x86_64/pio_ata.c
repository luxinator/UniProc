//
// Created by Lucas van Oosterhout on 7/6/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include <stdint.h>
#include <stdio.h>
#include "io.h"
#include <kernel/kernel.h>
#include <kernel/timer.h>

#define is_flag_set(v, flag) (((v) & (flag)) == (flag))

#define ATA_BUS_DATA           0 // 16-bit
#define ATA_BUS_ERROR          1 // 16-bit
#define ATA_BUS_FEATURES       1 // 16-bit
#define ATA_BUS_SECTOR_COUNT   2 // 16-bit
#define ATA_BUS_LBA_LOW        3 // 16-bit
#define ATA_BUS_LBA_MID        4 // 16-bit
#define ATA_BUS_LBA_HIGH       5 // 16-bit
#define ATA_BUS_DRIVE_SELECT   6 // 8-bit
#define ATA_BUS_STATUS         7 // 8-bit
#define ATA_BUS_COMMAND        7 // 8-bit
#define ATA_REG_SECCOUNT1      0x08 // 16-bit
#define ATA_REG_LBA3           0x09 // 16-bit
#define ATA_REG_LBA4           0x0A // 16-bit
#define ATA_REG_LBA5           0x0B // 16-bit

#define ATA_CONTROL_ALT_STATUS        0 // 8-bit
#define ATA_CONTROL_DEVICE_CONTROL    0 // 8-bit
#define ATA_CONTROL_DEVICE_REG        1 // 8-bit

#define PRIMARY_ATA_IO_BUS 0x1F0
#define PRIMARY_CONTROL_REG 0x3F6

#define SECONDARY_ATA_IO_BUS  0x170
#define SECONDARY_CONTROL_REG 0x376

// ToDo: Rewrite based on: https://wiki.osdev.org/PCI_IDE_Controller

typedef struct AtaBus {
  const char *name;
  uint16_t data_bus;
  uint16_t control;
} AtaBus;

AtaBus Primary = {
	.name="PRI",
	.data_bus = PRIMARY_ATA_IO_BUS,
	.control = PRIMARY_CONTROL_REG,
};

AtaBus Secondary = {
	.name="SEC",
	.data_bus = SECONDARY_ATA_IO_BUS,
	.control = SECONDARY_CONTROL_REG,
};

// -- ATA_BUS_ERROR
#define AddressMarkNotFount    0
#define TrackZeroNotFound      1 << 1
#define Aborted                1 << 2
#define MediaChangeRequest     1 << 3
#define IdNotFound             1 << 4
#define MediaChanged           1 << 5
#define UncorrectableData      1 << 6
#define BadBlockDetected       1 << 7

// -- ATA_BUS_DRIVE
#define BlockNumber0    0,
#define BlockNumber1    1 << 1
#define BlockNumber2    1 << 2
#define BlockNumber3    1 << 3
#define Drive           1 << 4
#define AlwaysOne       1 << 5
#define UsesLBA         1 << 6
#define AlwaysOneToo    1 << 7

// -- ATA_BUS_STATUS
#define Error        0
#define Index        1 << 1
#define Correct      1 << 2
#define DriveReady   1 << 3
#define OverlappedModeServiceRequest 1 << 4
#define DriveFault  1 << 5 // Does NOT set error
#define Ready       1 << 6
#define Busy        1 << 7

#define StopInterrupts  1 << 1
#define SoftReset       1 << 2 // Set (wait 5 millis) unset, to reset controller,
#define HighOrderBit    7 << 1 // Set to read High Order byte of LBA48 from io port

// -- ATA_CONTROL_DEVICE_REG
#define Drive0        0
#define Drive1        1 << 1
#define HeadSelect0   1 << 2
#define HeadSelect1   1 << 3
#define HeadSelect2   1 << 4
#define HeadSelect3   1 << 5
#define WriteGate     1 << 6 // Goes low when write is in progress,
#define None          1 << 7 // Reserved for controller with a floppy drive

// -- ATA_BUS_COMMAND commands
#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define      ATAPI_CMD_READ       0xA8
#define      ATAPI_CMD_EJECT      0x1B

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

inline bool drive_is_busy(AtaBus bus, bool slave) {
  uint8_t v = inb(bus.control | ((uint8_t)slave << 4));
  return is_flag_set(v, Busy);
}

void wait(AtaBus bus) {

}

void init_ata_bus(AtaBus bus) {
  outb(bus.control, 0x0);
  outb(bus.control, StopInterrupts);
}

void write_command_to_drive(uint16_t bus, int slave, uint8_t command) {

}

void select_drive(AtaBus bus, bool slave) {
  // select the drive on the bus
  //			        	slave bit     always 1    use LBA    always 1
  uint8_t select = 0x0 | (slave << 4) | AlwaysOne | UsesLBA | AlwaysOneToo;
  outb(bus.data_bus + ATA_BUS_DRIVE_SELECT, select);
  // Wait 400ns
  inb(bus.control);
  inb(bus.control);
  inb(bus.control);
  inb(bus.control);
}

void reset_bus(AtaBus bus) {
  outb(bus.control, SoftReset);
  sleep(5);
  outb(bus.control, StopInterrupts);
}

void write_command_to_bus(AtaBus bus, uint8_t cmd) {
  outb(bus.control, cmd);
}

uint16_t print_ata_error(AtaBus bus) {
  // ToDo Put somewhere
  uint16_t v = inw(bus.data_bus + ATA_BUS_ERROR);

  if (is_flag_set(v, AddressMarkNotFount)) printf("AddressMarkNotFount\n");
  if (is_flag_set(v, TrackZeroNotFound)) printf("TrackZeroNotFound\n");
  if (is_flag_set(v, Aborted)) printf("Aborted\n");
  if (is_flag_set(v, MediaChangeRequest)) printf("MediaChangeRequest\n");
  if (is_flag_set(v, IdNotFound)) printf("IdNotFound\n");
  if (is_flag_set(v, MediaChanged)) printf("MediaChanged\n");
  if (is_flag_set(v, UncorrectableData)) printf("UncorrectableData\n");
  if (is_flag_set(v, BadBlockDetected)) printf("BadBlockDetected\n");

  return v;
}

void print_status(uint16_t s) {
  printf("status:\n ");
  if (is_flag_set(s, Error)) printf("Error, ");
  if (is_flag_set(s, Index)) printf("Index, ");
  if (is_flag_set(s, Correct)) printf("Correct, ");
  if (is_flag_set(s, DriveReady)) printf("DriveReady, ");
  if (is_flag_set(s, OverlappedModeServiceRequest)) printf("OverlappedModeServiceRequest, ");
  if (is_flag_set(s, DriveFault)) printf("DriveFault, ");
  if (is_flag_set(s, Ready)) printf("Ready, ");
  if (is_flag_set(s, Busy)) printf("Busy, ");
  printf("\n");
}

void detect_drive(AtaBus bus, bool slave) {
  char *s = slave ? "SLAVE" : "MASTER";

  reset_bus(bus);
  select_drive(bus, slave);
  // Select drive
  // Do a IDENTIFY command
  outb(bus.data_bus + ATA_BUS_DRIVE_SELECT, 0xA0 | (slave << 4));
  outw(bus.data_bus + ATA_BUS_LBA_LOW, 0x0);
  outw(bus.data_bus + ATA_BUS_LBA_MID, 0x0);
  outw(bus.data_bus + ATA_BUS_LBA_HIGH, 0x0);
  outw(bus.data_bus + ATA_BUS_COMMAND, 0xEC); // IDENTIFY command

  uint8_t status = inb(bus.control + ATA_CONTROL_ALT_STATUS);
  if (status==0) {
	printf("No drive on %s:%s\n", bus.name, s);
	return;
  }
  printf("Found a drive %s-%s: %x\n", bus.name, s, status);

  // SATA (and ATAPI) drives should report an error immediately
  if (is_flag_set(status, Error)) {
	uint16_t flag = inw(bus.data_bus + ATA_BUS_LBA_MID);
	if (flag==0xeb14) {
	  printf("   is ATAPI drive\n");
	  return;
	} else if (flag==0xc33c) {
	  printf("   is SATA drive\n");
	  return;
	} else if (flag==0x9669) {
	  printf("   is SATAPI drive\n");
	  return;
	}
  }

  // ATA should be "busy" for a while
  while (is_flag_set(status, Busy)) {
	printf("Waiting on bus\n");
	status = inb(bus.data_bus + ATA_BUS_STATUS);
  }

  // Wait for the drive write its data to the output buffers
  status = inb(bus.control + ATA_CONTROL_ALT_STATUS);
  while (!is_flag_set(status, DriveReady) || !is_flag_set(status, Error)) {
	printf("Waiting on drive\n");
	status = inb(bus.control + ATA_CONTROL_ALT_STATUS);
  }

  if (is_flag_set(status, Error)) {
	printf("Error reading from drive: ");
	print_status(status);
	return;
  }

  outw(bus.data_bus + ATA_BUS_LBA_LOW, 0x0);
  outw(bus.data_bus + ATA_BUS_LBA_MID, 0x0);
  outw(bus.data_bus + ATA_BUS_LBA_HIGH, 0x0);
  uint16_t d = inw(bus.data_bus + ATA_BUS_DATA);
  if (d==0) {
	printf("   %s is not a hard disk", s);
	return;
  }

  outw(bus.data_bus + ATA_BUS_LBA_LOW, 83);
  outw(bus.data_bus + ATA_BUS_LBA_MID, 0x0);
  outw(bus.data_bus + ATA_BUS_LBA_HIGH, 0x0);
  d = inw(bus.data_bus + ATA_BUS_DATA);
  if (is_flag_set(d, 1 << 10)) {
	printf("   drive is LBA48 compatible\n");

	outw(bus.data_bus + ATA_BUS_LBA_LOW, 100);
	uint64_t sectors = (uint64_t)inw(bus.data_bus + ATA_BUS_DATA);

	outw(bus.data_bus + ATA_BUS_LBA_LOW, 101);
	sectors = sectors | (uint64_t)inw(bus.data_bus + ATA_BUS_DATA) << 16;

	outw(bus.data_bus + ATA_BUS_LBA_LOW, 101);
	sectors = sectors | (uint64_t)inw(bus.data_bus + ATA_BUS_DATA) << 32;

	outw(bus.data_bus + ATA_BUS_LBA_LOW, 101);
	sectors = sectors | (uint64_t)inw(bus.data_bus + ATA_BUS_DATA) << 48;

	printf("   drive has %d sectors: %d bytes (%d mbytes)\n", sectors, sectors*512, sectors*512/1024);

  } else {
	printf("   drive is LBA24 compatible\n");

	outw(bus.data_bus + ATA_BUS_LBA_LOW, 60);
	uint16_t low = inw(bus.data_bus + ATA_BUS_DATA);
	outw(bus.data_bus + ATA_BUS_LBA_LOW, 61);
	uint16_t high = inw(bus.data_bus + ATA_BUS_DATA);

	uint32_t sectors = high << 16 | low;
	printf("   drive has %d sectors: %d bytes (%d kbytes)\n", sectors, sectors*512, sectors*512/1024);
  }



//
//  uint8_t sig_l = inb(PRIMARY_ATA_IO_BUS + ATA_BUS_LBA_MID);
//  uint8_t sig_h = inb(PRIMARY_ATA_IO_BUS + ATA_BUS_LBA_HIGH);
//
//  if (sig_l==0x14 && sig_h==0xEB) printf("ATADEV_PATAPI\n");
//  if (sig_l==0x69 && sig_h==0x96) printf("ATADEV_SATAPI\n");
//  if (sig_l==0 && sig_h==0) printf("ATADEV_PATA\n");
//  if (sig_l==0x3c && sig_h==0xc3) printf("ATADEV_SATA\n");
//  printf("ATADEV_UNKNOWN\n");

}

void start_disk_drives() {
  detect_drive(Primary, false);
  detect_drive(Primary, true);

  detect_drive(Secondary, false);
  detect_drive(Secondary, true);
}