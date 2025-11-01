#pragma once

#include <simavr/sim/sim_irq.h>

#include <memory>

template <typename T>
concept I2cDevice = requires(T t) {
  { t.input_irq() } -> std::same_as<std::shared_ptr<avr_irq_t>>;
  { t.output_irq() } -> std::same_as<std::shared_ptr<avr_irq_t>>;
};
