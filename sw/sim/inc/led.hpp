#pragma once

#include <simavr/sim/avr_ioport.h>
#include <simavr/sim/sim_irq.h>
#include <simavr/sim_avr.h>
#include <simavr/sim_time.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>

#include "mcu.hpp"

template <AvrMcu MCU, uint32_t PORT, int IDX>
class Led {
 private:
  MCU mcu_;
  avr_irq_t *irq_;
  std::shared_ptr<avr_t> avr_;
  uint32_t last_state_;
  uint64_t last_ts_;
  std::optional<std::function<void(uint32_t)>> on_change_;

  static void on_change(avr_irq_t *irq, uint32_t value, void *param) {
    Self *self = static_cast<Self *>(param);
    if (self->last_state_ != value) {
      if (auto callback = self->on_change_) {
        (*callback)(value);
      }
      self->last_ts_ = self->avr_->cycle;
      self->last_state_ = value;
    }
  }

 public:
  using Self = Led<MCU, PORT, IDX>;

  explicit Led(std::shared_ptr<MCU> avr) noexcept
      : irq_(avr_io_getirq(&*avr->avr(), AVR_IOCTL_IOPORT_GETIRQ(PORT), IDX)),
        avr_(avr->avr()),
        last_state_(-1),
        last_ts_(0),
        on_change_(std::nullopt) {
    avr_irq_register_notify(&*irq_, Self::on_change, this);
  }

  virtual ~Led() noexcept {
    avr_irq_unregister_notify(&*irq_, Self::on_change, this);
  }

  void on_change(std::function<void(uint32_t)> callback) noexcept {
    on_change_ = std::make_optional(callback);
  }

  uint64_t last_ts() const noexcept { return last_ts_; }
};
