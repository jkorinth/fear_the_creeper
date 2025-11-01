#pragma once
#include <simavr/sim/sim_avr.h>

#include <concepts>
#include <memory>
#include <string>

#include "attiny85.hpp"
#include "i2c_device.hpp"

template <typename T>
concept AvrMcu = requires(T t, uint64_t cycles, size_t idx) {
  { t.mcu() } -> std::same_as<std::string>;
  { t.avr() } -> std::same_as<std::shared_ptr<avr_t>>;
  { t.cycle() } -> std::unsigned_integral;
  { t.frequency() } -> std::unsigned_integral;
  { t.pc() } -> std::unsigned_integral;
  { t.step() } -> std::same_as<CpuState>;
  { t.cycles_to_nsec(cycles) } -> std::same_as<uint64_t>;
};
