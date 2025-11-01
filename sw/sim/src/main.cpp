#include <signal.h>
#include <simavr/avr/avr_mcu_section.h>
#include <simavr/sim/sim_avr.h>
#include <simavr/sim/sim_elf.h>
#include <simavr/sim/sim_gdb.h>
#include <termios.h>
#include <unistd.h>

#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>

#include "attiny85.hpp"
#include "iis328dq.hpp"
#include "led.hpp"
#include "log.h"

#undef F_CPU
#define F_CPU 8000000

static volatile int keep_running = 1;

static void int_handler(int) { keep_running = 0; }

static int kb() {
  struct timeval tv = {0L, 0L};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  return select(1, &fds, NULL, NULL, &tv) > 0;
}

static int simulate() {
  std::shared_ptr<ATtiny85<F_CPU>> avr = std::make_shared<ATtiny85<F_CPU>>();
  auto iis = IIS328DQ(avr);

  Led<ATtiny85<F_CPU>, 'B', 4> leds(avr);
  leds.on_change([&avr, &leds](uint32_t value) {
    uint64_t delta_cycles = avr->cycle() - leds.last_ts();
    trace(leds, "+{:10.4f}ms: LED changed to {}",
          avr->cycles_to_nsec(delta_cycles) / 1000000.0f,
          value ? "HIGH" : "LOW");
  });

  info(sim, "firmware loaded for {} at {} Hz", avr->mcu(), avr->frequency());
  info(sim, "ATtiny85 simulation started");

  uint64_t cycle_count = 0;

  while (keep_running) {
    auto state = avr->step();

    if (state == CpuState::DONE || state == CpuState::CRASHED) {
      info(sim, "Simulation ended: state={}", std::format("{}", state));
      break;
    }

    cycle_count++;

    if (kb()) {
      getchar();
      break;
    }
  }

  info(sim, "Simulation ran for {} cycles.", cycle_count);
  return 0;
}

static void set_nb(int enable) {
  static struct termios oldt, newt;

  if (enable) {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  } else {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  }
}

int main(int argc, const char **argv) {
  int result = 0;
  init_logging(argc, argv);
  spdlog::set_level(spdlog::level::trace);
  signal(SIGINT, int_handler);
  set_nb(true);
  info(main, "starting simulation ...");
  result = simulate();
  set_nb(false);
  return result;
}
