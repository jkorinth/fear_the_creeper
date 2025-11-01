#pragma once

#include <simavr/sim/avr_twi.h>
#include <simavr/sim/avr_usi.h>
#include <simavr/sim/sim_irq.h>

#include <array>
#include <memory>
#include <optional>
#include <span>

#include "log.h"
#include "mcu.hpp"

enum class Register : uint8_t {
  WhoAmI = 0x0f,
  CtrlReg1 = 0x20,
  CtrlReg2 = 0x21,
  CtrlReg3 = 0x22,
  CtrlReg4 = 0x23,
  CtrlReg5 = 0x24,
  HpFilterReset = 0x25,
  Reference = 0x26,
  StatusReg = 0x27,
  Out_X_L = 0x28,
  Out_X_H = 0x29,
  Out_Y_L = 0x2A,
  Out_Y_H = 0x2B,
  Out_Z_L = 0x2C,
  Out_Z_H = 0x2D,
  Int1Cfg = 0x30,
  Int1Src = 0x31,
  Int1Ths = 0x32,
  Int1Duration = 0x33,
  Int2Cfg = 0x34,
  Int2Src = 0x35,
  Int2Ths = 0x36,
  Int2Duration = 0x37,
};

inline static std::optional<Register> into_reg(uint8_t addr) noexcept {
  switch (addr) {
    case static_cast<uint8_t>(Register::WhoAmI):
      return Register::WhoAmI;
    case static_cast<uint8_t>(Register::CtrlReg1):
      return Register::CtrlReg1;
    case static_cast<uint8_t>(Register::CtrlReg2):
      return Register::CtrlReg2;
    case static_cast<uint8_t>(Register::CtrlReg3):
      return Register::CtrlReg3;
    case static_cast<uint8_t>(Register::CtrlReg4):
      return Register::CtrlReg4;
    case static_cast<uint8_t>(Register::CtrlReg5):
      return Register::CtrlReg5;
    case static_cast<uint8_t>(Register::HpFilterReset):
      return Register::HpFilterReset;
    case static_cast<uint8_t>(Register::Reference):
      return Register::Reference;
    case static_cast<uint8_t>(Register::StatusReg):
      return Register::StatusReg;
    case static_cast<uint8_t>(Register::Out_X_L):
      return Register::Out_X_L;
    case static_cast<uint8_t>(Register::Out_X_H):
      return Register::Out_X_H;
    case static_cast<uint8_t>(Register::Out_Y_L):
      return Register::Out_Y_L;
    case static_cast<uint8_t>(Register::Out_Y_H):
      return Register::Out_Y_H;
    case static_cast<uint8_t>(Register::Out_Z_L):
      return Register::Out_Z_L;
    case static_cast<uint8_t>(Register::Out_Z_H):
      return Register::Out_Z_H;
    case static_cast<uint8_t>(Register::Int1Cfg):
      return Register::Int1Cfg;
    case static_cast<uint8_t>(Register::Int1Src):
      return Register::Int1Src;
    case static_cast<uint8_t>(Register::Int1Ths):
      return Register::Int1Ths;
    case static_cast<uint8_t>(Register::Int1Duration):
      return Register::Int1Duration;
    case static_cast<uint8_t>(Register::Int2Cfg):
      return Register::Int2Cfg;
    case static_cast<uint8_t>(Register::Int2Src):
      return Register::Int2Src;
    case static_cast<uint8_t>(Register::Int2Ths):
      return Register::Int2Ths;
    case static_cast<uint8_t>(Register::Int2Duration):
      return Register::Int2Duration;
    default:
      return std::nullopt;
  }
}

template <AvrMcu MCU>
class IIS328DQ {
  std::array<std::shared_ptr<avr_irq_t>, 2> irq_;
  uint8_t addr_base_;
  uint8_t addr_mask_;
  std::optional<Register> reg_addr_;
  std::array<std::optional<uint8_t>, 0x40> reg_map_;
  bool selected_;

  using Self = IIS328DQ<MCU>;

  static void iis328dq_in_hook(struct avr_irq_t *irq, uint32_t value,
                               void *param) noexcept;

 public:
  explicit IIS328DQ(std::shared_ptr<MCU> mcu) noexcept;
  virtual ~IIS328DQ() noexcept = default;

  IIS328DQ(const IIS328DQ<MCU> &) = delete;
  IIS328DQ(IIS328DQ<MCU> &&) = delete;

  IIS328DQ operator=(const IIS328DQ<MCU> &) = delete;
  IIS328DQ operator=(IIS328DQ<MCU> &&) = delete;

  std::shared_ptr<avr_irq_t> input_irq() noexcept { return irq_[0]; }
  std::shared_ptr<avr_irq_t> output_irq() noexcept { return irq_[1]; }
};

static constexpr std::array<const char *, 2> _irq_names{
    "8>iis328dq.out",
    "24<iis328dq.in",
};

template <AvrMcu MCU>
IIS328DQ<MCU>::IIS328DQ(std::shared_ptr<MCU> mcu) noexcept
    : irq_{{mcu->acquire_irq(_irq_names[0]), mcu->acquire_irq(_irq_names[1])}} {
  avr_irq_register_notify(&*(this->irq_[0]), IIS328DQ<MCU>::iis328dq_in_hook,
                          this);
  avr_irq_register_notify(&*(this->irq_[1]), IIS328DQ<MCU>::iis328dq_in_hook,
                          this);
  trace(iis328dq, "registered irq notify");
  mcu->attach_i2c(0, this);
  trace(iis328dq, "attached to mcu");
}

template <AvrMcu MCU>
void IIS328DQ<MCU>::iis328dq_in_hook(struct avr_irq_t *irq, uint32_t value,
                                     void *param) noexcept {
  Self *self = static_cast<Self *>(param);
  avr_twi_msg_irq_t v;
  v.u.v = value;

  if (v.u.twi.msg & TWI_COND_STOP) {
    dbg(i2c, "received stop");
    self->selected_ = false;
    self->reg_addr_ = std::nullopt;
  }

  if (v.u.twi.msg & TWI_COND_STOP) {
    dbg(i2c, "received stop2");
    self->selected_ = false;
    if ((self->addr_base_ & ~self->addr_mask_) ==
        (v.u.twi.addr & ~self->addr_mask_)) {
      self->selected_ = true;
      dbg(i2c, "raising irq input");
      avr_raise_irq(&*(self->irq_[0]),
                    avr_twi_irq_msg(TWI_COND_ACK,
                                    self->addr_base_ & ~self->addr_mask_, 1));
    }
  }

  if (self->selected_ && v.u.twi.msg & TWI_COND_WRITE) {
    dbg(i2c, "write cond");
    avr_raise_irq(
        &*(self->irq_[0]),
        avr_twi_irq_msg(TWI_COND_ACK, self->addr_base_ & ~self->addr_mask_, 1));
    if (self->reg_addr_.has_value()) {
      if (self->reg_map_[static_cast<uint8_t>(*self->reg_addr_)].has_value()) {
        self->reg_map_[static_cast<uint8_t>(*self->reg_addr_)] =
            std::make_optional(static_cast<uint8_t>(v.u.twi.data));
      }
    } else {
      self->reg_addr_ = into_reg(static_cast<uint8_t>(v.u.twi.data));
    }
  }

  if (self->selected_ && v.u.twi.msg & TWI_COND_READ &&
      self->reg_addr_.has_value()) {
    dbg(i2c, "read cond");
    avr_raise_irq(&*self->irq_[0],
                  avr_twi_irq_msg(
                      TWI_COND_READ, self->selected_,
                      *self->reg_map_[static_cast<uint8_t>(*self->reg_addr_)]));
  }
}
