#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H
#include <stdint.h>
#include <stdbool.h>

/** outb:
*  Sends the given data to the given I/O port.
*
*  @param port The I/O port to send the data to
*  @param data The data to send to the I/O port
*/
static inline void outb(uint16_t port, uint8_t data) {
  __asm__ volatile ("outb %0, %1" : : "a" (data), "Nd" (port): "memory");
}

/** outb:
*  Receive data from the given I/O port.
*
*  @param port The I/O port to send the data to
*/
static inline uint8_t inb(uint16_t port) {
  uint8_t value;
  __asm__ volatile ("inb %1, %0" : "=a" (value) : "Nd" (port): "memory");
  return value;
}

static inline void io_wait(void) {
  outb(0x80, 0);
}

static inline bool are_interrupts_enabled() {
  unsigned long flags;
  __asm__ volatile ( "pushf\n\t"
                     "pop %0"
      : "=g"(flags));
  return flags & (1 << 9);
}

static inline unsigned long save_irqdisable(void) {
  unsigned long flags;
  __asm__ volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
  return flags;
}

static inline void irqrestore(unsigned long flags) {
  __asm__ ("push %0\n\tpopf" : : "rm"(flags) : "memory", "cc");
}

//
//static void intended_usage(void) {
//  unsigned long f = save_irqdisable();
//  do_whatever_without_irqs();
//  irqrestore(f);
//}


#endif /* INCLUDE_IO_H */

