#include <leds.h>
#include <simavr/sim/avr_ioport.h>
#include <simavr/sim/sim_avr.h>
#include <simavr/sim/sim_irq.h>
#include <simavr/sim/sim_time.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

typedef struct leds {
  avr_irq_t *irq;
  avr_t *avr;
  uint32_t last_state;
  uint64_t last_ts;
} leds_t;

static void leds_on_change(struct avr_irq_t *irq, uint32_t value, void *param) {
  leds_t *leds = static_cast<leds_t *>(param);

  if (value != leds->last_state) {
    uint64_t delta_cycles = leds->avr->cycle - leds->last_ts;
    leds->last_ts = leds->avr->cycle;
    printf("\n+%10.4fms: [LED] LED changed to %s\n",
           avr_cycles_to_nsec(leds->avr, delta_cycles) / 1000000.0f,
           // leds->avr->cycle,
           value ? "HIGH" : "LOW");
  }
}

void *init_leds(avr_t *avr) {
  leds_t *leds = static_cast<leds_t *>(calloc(1, sizeof(leds_t)));
  leds->avr = avr;
  leds->last_state = -1;
  leds->irq = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 4);
  avr_irq_register_notify(leds->irq, leds_on_change, leds);
  return leds;
}
