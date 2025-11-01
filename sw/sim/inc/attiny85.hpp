#pragma once
#include <simavr/sim/avr_twi.h>
#include <simavr/sim/avr_usi.h>
#include <simavr/sim/sim_elf.h>
#include <simavr/sim/sim_time.h>

#include <cstdint>
#include <cstdlib>
#include <format>
#include <memory>
#include <stdexcept>

#include "i2c_device.hpp"
#include "log.h"

static constexpr auto FW_FILE = "../.pio/build/attiny85/firmware.elf";

enum class CpuState {
  LIMBO = cpu_Limbo,
  STOPPED = cpu_Stopped,
  RUNNING = cpu_Running,
  SLEEPING = cpu_Sleeping,
  STEP = cpu_Step,
  STEPDONE = cpu_StepDone,
  DONE = cpu_Done,
  CRASHED = cpu_Crashed,
};

template <>
struct std::formatter<CpuState> : std::formatter<std::string_view> {
  static constexpr std::string_view to_string(CpuState state) {
    switch (state) {
      case CpuState::LIMBO:
        return "LIMBO";
      case CpuState::STOPPED:
        return "STOPPED";
      case CpuState::RUNNING:
        return "RUNNING";
      case CpuState::SLEEPING:
        return "SLEEPING";
      case CpuState::STEP:
        return "STEP";
      case CpuState::STEPDONE:
        return "STEPDONE";
      case CpuState::DONE:
        return "DONE";
      case CpuState::CRASHED:
        return "CRASHED";
      default:
        return "unknown";
    }
  }

  auto format(CpuState state, std::format_context& ctx) const {
    return std::formatter<string_view>::format(to_string(state), ctx);
  }
};

template <uint32_t F_CPU>
struct ATtiny85 {
 private:
  std::unique_ptr<elf_firmware_t> fw_;
  std::shared_ptr<avr_t> avr_;
  avr_t* avr_raw() const noexcept { return &*avr_; }

 public:
  explicit ATtiny85(const char* fw_file = FW_FILE)
      : fw_(std::make_unique<elf_firmware_t>()) {
    int ret = elf_read_firmware(fw_file, &*fw_);
    if (ret) {
      throw std::runtime_error(
          std::format("could not read firmware {}", fw_file));
    }

    avr_ = std::shared_ptr<avr_t>(avr_make_mcu_by_name("attiny85"), ::free);
    if (!avr_) {
      throw std::runtime_error("could not create attiny85 instance");
    }
    avr_->frequency = F_CPU;
    ret = avr_init(avr_raw());
    if (ret) {
      throw std::runtime_error(
          std::format("could init attiny85 instance: {}", ret));
    }
    avr_load_firmware(avr_raw(), &*fw_);
  }

  ATtiny85(const ATtiny85<F_CPU>& other) = delete;
  ATtiny85(ATtiny85<F_CPU>&& other) = delete;

  ATtiny85<F_CPU> operator=(const ATtiny85<F_CPU>&) = delete;
  ATtiny85<F_CPU> operator=(ATtiny85<F_CPU>&&) = delete;

  virtual ~ATtiny85() noexcept { avr_terminate(avr_raw()); }

  CpuState step() noexcept { return static_cast<CpuState>(avr_run(avr_raw())); }

  uint64_t cycle() const noexcept { return avr_->cycle; }

  std::shared_ptr<avr_t> avr() noexcept { return avr_; }

  std::string mcu() const noexcept { return std::string(fw_->mmcu); }

  uint32_t frequency() const noexcept { return avr_->frequency; }

  uint64_t pc() const noexcept { return avr_->pc; }

  uint64_t cycles_to_nsec(uint64_t cycles) {
    return avr_cycles_to_nsec(&*avr_, cycles);
  }

  template <I2cDevice DEV>
  bool attach_i2c(size_t idx, DEV* dev) {
    avr_t* avr = &*this->avr_;

    avr_irq_t* twi_status_irq =
        avr_io_getirq(avr, AVR_IOCTL_USI_GETIRQ(), TWI_IRQ_STATUS);
    avr_irq_t* twi_data_irq =
        avr_io_getirq(avr, AVR_IOCTL_USI_GETIRQ(), USI_IRQ_DI);

    avr_irq_t* dev_input = dev->input_irq().get();
    avr_irq_t* dev_output = dev->output_irq().get();

    trace(attiny85, "connecting input {} to AVR {}",
          static_cast<void*>(dev_input), static_cast<void*>(twi_status_irq));
    avr_connect_irq(twi_status_irq, dev_input);
    trace(attingy85, "connected");

    trace(attiny85, "connecting AVR {} to output {}",
          static_cast<void*>(dev_output), static_cast<void*>(twi_data_irq));
    avr_connect_irq(twi_data_irq, dev_output);
    trace(attingy85, "connected");

    avr_ioctl(avr, AVR_IOCTL_USI_SET_ADDR, 0x32);
    return false;
  }

  std::shared_ptr<avr_irq_t> acquire_irq(const char* name = nullptr) {
    static size_t num = 0;
    auto irq = avr_alloc_irq(&avr_->irq_pool, num++, 1, &name);
    if (!irq) {
      throw std::runtime_error("could not allocate irq!");
    } else {
      trace(irq, "acquire irq #{}", irq->irq);
    }
    return std::shared_ptr<avr_irq_t>(irq,
                                      [](auto irq) { avr_free_irq(irq, 1); });
  }
};
