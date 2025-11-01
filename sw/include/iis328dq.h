// IIS328DQ Register Definitions and Bit Masks
#pragma once

#include <stdint.h>

// ============================================================================
// I2C Address
// ============================================================================
#define IIS328DQ_I2C_ADDR_7BIT    0x19  // 7-bit address (SA0 = 0)
#define IIS328DQ_I2C_ADDR_WRITE   0x32  // 8-bit write address
#define IIS328DQ_I2C_ADDR_READ    0x33  // 8-bit read address

// ============================================================================
// Register Addresses
// ============================================================================
#define IIS328DQ_WHO_AM_I         0x0F
#define IIS328DQ_CTRL_REG1        0x20
#define IIS328DQ_CTRL_REG2        0x21
#define IIS328DQ_CTRL_REG3        0x22
#define IIS328DQ_CTRL_REG4        0x23
#define IIS328DQ_CTRL_REG5        0x24
#define IIS328DQ_HP_FILTER_RESET  0x25
#define IIS328DQ_REFERENCE        0x26
#define IIS328DQ_STATUS_REG       0x27
#define IIS328DQ_OUT_X_L          0x28
#define IIS328DQ_OUT_X_H          0x29
#define IIS328DQ_OUT_Y_L          0x2A
#define IIS328DQ_OUT_Y_H          0x2B
#define IIS328DQ_OUT_Z_L          0x2C
#define IIS328DQ_OUT_Z_H          0x2D
#define IIS328DQ_INT1_CFG         0x30
#define IIS328DQ_INT1_SRC         0x31
#define IIS328DQ_INT1_THS         0x32
#define IIS328DQ_INT1_DURATION    0x33
#define IIS328DQ_INT2_CFG         0x34
#define IIS328DQ_INT2_SRC         0x35
#define IIS328DQ_INT2_THS         0x36
#define IIS328DQ_INT2_DURATION    0x37

// ============================================================================
// CTRL_REG1 (0x20) - Power Control and Data Rate
// ============================================================================
#define CTRL_REG1_PD              0x80  // Power-down control (0=power-down, 1=active)
#define CTRL_REG1_DR1             0x40  // Data rate bit 1
#define CTRL_REG1_DR0             0x20  // Data rate bit 0
#define CTRL_REG1_ZEN             0x04  // Z-axis enable
#define CTRL_REG1_YEN             0x02  // Y-axis enable
#define CTRL_REG1_XEN             0x01  // X-axis enable

// Data rate configurations (DR1:DR0)
#define CTRL_REG1_ODR_50HZ        0x00  // 50 Hz output data rate
#define CTRL_REG1_ODR_100HZ       0x20  // 100 Hz output data rate
#define CTRL_REG1_ODR_400HZ       0x40  // 400 Hz output data rate
#define CTRL_REG1_ODR_1000HZ      0x60  // 1000 Hz output data rate

// ============================================================================
// CTRL_REG2 (0x21) - High-Pass Filter Configuration
// ============================================================================
#define CTRL_REG2_BOOT            0x80  // Reboot memory content
#define CTRL_REG2_HPM1            0x40  // High-pass filter mode bit 1
#define CTRL_REG2_HPM0            0x20  // High-pass filter mode bit 0
#define CTRL_REG2_FDS             0x10  // Filtered data selection
#define CTRL_REG2_HPEN2           0x08  // High-pass filter enabled for interrupt 2
#define CTRL_REG2_HPEN1           0x04  // High-pass filter enabled for interrupt 1
#define CTRL_REG2_HPCF1           0x02  // High-pass filter cutoff frequency bit 1
#define CTRL_REG2_HPCF0           0x01  // High-pass filter cutoff frequency bit 0

// High-pass filter modes (HPM1:HPM0)
#define CTRL_REG2_HPM_NORMAL      0x00  // Normal mode (reset reading REFERENCE)
#define CTRL_REG2_HPM_REFERENCE   0x20  // Reference signal for filtering
#define CTRL_REG2_HPM_NORMAL2     0x40  // Normal mode
#define CTRL_REG2_HPM_AUTORESET   0x60  // Autoreset on interrupt

// ============================================================================
// CTRL_REG3 (0x22) - Interrupt Control
// ============================================================================
#define CTRL_REG3_IHL             0x80  // Interrupt active high/low (0=high, 1=low)
#define CTRL_REG3_PP_OD           0x40  // Push-pull/Open-drain (0=push-pull, 1=open-drain)
#define CTRL_REG3_LIR2            0x20  // Latch interrupt request on INT2
#define CTRL_REG3_I2_CFG1         0x10  // Data signal on INT2 pad bit 1
#define CTRL_REG3_I2_CFG0         0x08  // Data signal on INT2 pad bit 0
#define CTRL_REG3_LIR1            0x04  // Latch interrupt request on INT1
#define CTRL_REG3_I1_CFG1         0x02  // Data signal on INT1 pad bit 1
#define CTRL_REG3_I1_CFG0         0x01  // Data signal on INT1 pad bit 0

// INT1 configurations (I1_CFG1:I1_CFG0)
#define CTRL_REG3_INT1_SRC_GND    0x00  // INT1 source is GND
#define CTRL_REG3_INT1_SRC_INT1   0x01  // Interrupt 1 on INT1
#define CTRL_REG3_INT1_SRC_INT2   0x02  // Interrupt 2 on INT1
#define CTRL_REG3_INT1_SRC_DR     0x03  // Data ready on INT1

// INT2 configurations (I2_CFG1:I2_CFG0)
#define CTRL_REG3_INT2_SRC_GND    0x00  // INT2 source is GND
#define CTRL_REG3_INT2_SRC_INT1   0x08  // Interrupt 1 on INT2
#define CTRL_REG3_INT2_SRC_INT2   0x10  // Interrupt 2 on INT2
#define CTRL_REG3_INT2_SRC_DR     0x18  // Data ready on INT2

// ============================================================================
// CTRL_REG4 (0x23) - Data Format and Self-Test
// ============================================================================
#define CTRL_REG4_BDU             0x80  // Block data update (0=continuous, 1=not updated until read)
#define CTRL_REG4_BLE             0x40  // Big/little endian (0=little, 1=big)
#define CTRL_REG4_FS1             0x20  // Full-scale selection bit 1
#define CTRL_REG4_FS0             0x10  // Full-scale selection bit 0
#define CTRL_REG4_STSIGN          0x08  // Self-test sign (0=plus, 1=minus)
#define CTRL_REG4_ST              0x02  // Self-test enable (0=disabled, 1=enabled)
#define CTRL_REG4_SIM             0x01  // SPI serial interface mode (0=4-wire, 1=3-wire)

// Full-scale selections (FS1:FS0)
#define CTRL_REG4_FS_2G           0x00  // ±2g full scale
#define CTRL_REG4_FS_4G           0x10  // ±4g full scale
#define CTRL_REG4_FS_8G           0x30  // ±8g full scale

// ============================================================================
// CTRL_REG5 (0x24) - Sleep-to-Wake and FIFO
// ============================================================================
#define CTRL_REG5_TURNON1         0x02  // Turn-on mode bit 1
#define CTRL_REG5_TURNON0         0x01  // Turn-on mode bit 0

// ============================================================================
// Interrupt Configuration Registers (INT1_CFG, INT2_CFG)
// ============================================================================
#define INT_CFG_AOI               0x80  // AND/OR combination (0=OR, 1=AND)
#define INT_CFG_6D                0x40  // 6-direction detection enable
#define INT_CFG_ZHIE              0x20  // Z high event enable
#define INT_CFG_ZLIE              0x10  // Z low event enable
#define INT_CFG_YHIE              0x08  // Y high event enable
#define INT_CFG_YLIE              0x04  // Y low event enable
#define INT_CFG_XHIE              0x02  // X high event enable
#define INT_CFG_XLIE              0x01  // X low event enable

// ============================================================================
// Interrupt Source Registers (INT1_SRC, INT2_SRC) - Read Only
// ============================================================================
#define INT_SRC_IA                0x40  // Interrupt active
#define INT_SRC_ZH                0x20  // Z high event
#define INT_SRC_ZL                0x10  // Z low event
#define INT_SRC_YH                0x08  // Y high event
#define INT_SRC_YL                0x04  // Y low event
#define INT_SRC_XH                0x02  // X high event
#define INT_SRC_XL                0x01  // X low event

// ============================================================================
// Status Register Bits
// ============================================================================
#define STATUS_ZYXOR              0x80  // X, Y, Z-axis data overrun
#define STATUS_ZOR                0x40  // Z-axis data overrun
#define STATUS_YOR                0x20  // Y-axis data overrun
#define STATUS_XOR                0x10  // X-axis data overrun
#define STATUS_ZYXDA              0x08  // X, Y, Z-axis new data available
#define STATUS_ZDA                0x04  // Z-axis new data available
#define STATUS_YDA                0x02  // Y-axis new data available
#define STATUS_XDA                0x01  // X-axis new data available

// ============================================================================
// Configuration Structure for Register/Value Pairs
// ============================================================================
struct IIS328DQ_RegConfig {
    uint8_t reg;
    uint8_t value;
};

// ============================================================================
// Initialization Array for Shake Detection
// Configuration: 100 Hz, ±2g, All axes, High-pass filter, INT2 active-high
// Threshold: 256 mg (0.25g), Duration: 20 ms
// ============================================================================
static const IIS328DQ_RegConfig iis328dq_shake_init[] = {
    // Power on, 100 Hz ODR, enable all axes
    { IIS328DQ_CTRL_REG1, 
      CTRL_REG1_PD | CTRL_REG1_ODR_100HZ | 
      CTRL_REG1_XEN | CTRL_REG1_YEN | CTRL_REG1_ZEN },
    
    // Enable high-pass filter for interrupt 2, cutoff ~2 Hz @ 100 Hz ODR
    { IIS328DQ_CTRL_REG2, 
      CTRL_REG2_HPEN1 | CTRL_REG2_HPM_NORMAL },
    
    // active-low, push-pull
    { IIS328DQ_CTRL_REG3, /*CTRL_REG3_LIR1 |*/ CTRL_REG3_IHL },
    
    // ±2g full scale, continuous update, little endian
    { IIS328DQ_CTRL_REG4, CTRL_REG4_FS_2G },
    
    // Disable sleep-to-wake and FIFO
    { IIS328DQ_CTRL_REG5, 0x00 },
    
    // Configure interrupt 1: OR combination, enable all axes high events
    { IIS328DQ_INT1_CFG, INT_CFG_ZHIE | INT_CFG_YHIE | INT_CFG_XHIE },
    
    // Set threshold to 16 LSB = 256 mg (at ±2g scale)
    { IIS328DQ_INT1_THS, 0xB },
    
    // Set duration to 1 samples = 10 ms @ 100 Hz
    { IIS328DQ_INT1_DURATION, 0x02 },

    // Configure interrupt 2: disabled
    { IIS328DQ_INT2_CFG, 0x00 },
    { IIS328DQ_INT2_THS, 0x00 },
    { IIS328DQ_INT2_DURATION, 0x00 },
};

// Number of initialization register pairs
#define IIS328DQ_INIT_COUNT (sizeof(iis328dq_shake_init) / sizeof(IIS328DQ_RegConfig))
