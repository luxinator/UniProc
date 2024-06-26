#ifndef SRC_ARCH_X86_64_SERIAL_H_
#define SRC_ARCH_X86_64_SERIAL_H_

#include "io.h"
#include <ctype.h>
#include <stdbool.h>

#define COM_PORT_1 0x3F8
#define COM_PORT_2 0x2F8

/**
 * Initializes a com port, a com port will trigger an interrupt when new data is presented.
 * The port transceive specs: 115200 baud, 8-bits, no-parity, one stop-bit
 * @param com_port
 * @return 0 on success
 */
static bool init_serial(uint16_t com_port) {
  // Example code from: https://wiki.osdev.org/Serial_ports
  outb(com_port + 1, 0x00);    // Disable all interrupts
  outb(com_port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  outb(com_port + 0, 0x01);    // Set divisor to 1 (lo byte) 115200 baud
  outb(com_port + 1, 0x00);    //                  (hi byte)
  outb(com_port + 3, 0x03);    // 8 bits, no parity, one stop bit
  outb(com_port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outb(com_port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
  outb(com_port + 4, 0x1E);    // Set in loopback mode, test the serial chip
  outb(com_port + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

  // Check if serial is faulty (i.e: not same byte as sent)
  if (inb(com_port + 0) != 0xAE) {
    return false;
  }

  // If serial is not faulty set it in normal operation mode
  // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
  outb(com_port + 4, 0x0F);
  return true;
}

/**
 * Check com port for data
 * @param com_port
 */
bool serial_received(uint16_t com_port) {
  return (inb(com_port + 5) & 1) != 0;
}

/**
 * Read a byte from the com port, note waits on data
 * @param com_port
 * @return a byte
 */
uint8_t read_serial(uint16_t com_port) {
  while (!serial_received(com_port));
  return inb(com_port);
}

/**
 * Function to check if data can me send over the comport
 * @param com_port
 * @return 0 == NOT EMPTY
 */
bool is_transmit_empty(uint16_t com_port) {
  return (inb(com_port + 5) & 0x20) != 0;
}

void write_serial(uint16_t com_port, uint8_t a) {
  while (!is_transmit_empty(com_port));

  outb(com_port, a);
}

#endif //SRC_ARCH_X86_64_SERIAL_H_
