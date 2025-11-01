#pragma once

#include <spdlog/cfg/argv.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>

inline static auto get_logger(const char *name)
    -> std::shared_ptr<spdlog::logger> {
  auto logger = spdlog::get(name);
  if (!logger) {
    return spdlog::create<spdlog::sinks::stdout_color_sink_mt>(name);
  }
  return logger;
}

#define trace(subsystem, ...) get_logger(#subsystem)->trace(__VA_ARGS__)

#define dbg(subsystem, ...) get_logger(#subsystem)->debug(__VA_ARGS__)

#define info(subsystem, ...) get_logger(#subsystem)->info(__VA_ARGS__)

#define warn(subsystem, ...) get_logger(#subsystem)->warn(__VA_ARGS__)

#define error(subsystem, ...) get_logger(#subsystem)->error(__VA_ARGS__)

inline void init_logging(int argc, const char **argv) {
  spdlog::cfg::load_argv_levels(argc, argv);
  spdlog::set_pattern("%^[%H:%M:%S.%e] [%L] [%n] %v%$");
}
