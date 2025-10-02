#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <simavr/sim/sim_avr.h>
#include <simavr/sim/sim_elf.h>
#include <simavr/sim/sim_gdb.h>
#include <simavr/avr/avr_mcu_section.h>
#include "leds.h"

#undef F_CPU
#define F_CPU 8000000

static volatile int keep_running = 1;

static void int_handler(int) {
	keep_running =  0;
}

static int kb()
{
	struct timeval tv = {0L, 0L};
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	return select(1, &fds, NULL, NULL, &tv) > 0;
}

static int load_firmware(const char *filename, elf_firmware_t *fw)
{
	int ret = 0;
	std::cout << "Loading firmware " << filename << " ..." << std::endl;
	if ((ret = elf_read_firmware(filename, fw)) != 0) {
		std::cerr << "could not load firmware: " << ret << std::endl;
		return ret;
	}
	return ret;
}

static int simulate()
{
	int ret = 0;
	elf_firmware_t *fw = static_cast<elf_firmware_t *>(calloc(1, sizeof(*fw)));
	if (! fw) {
		std::cerr << "ERROR: could not allocate memory for firmware!" << std::endl;
		return -1;
	}

	if ((ret = load_firmware("../.pio/build/attiny85/firmware.elf", fw)) != 0) {
		return ret;
	}

	avr_t *avr = avr_make_mcu_by_name("attiny85");
	if (! avr) {
		std::cerr << "failed to create attiny85 instance" << std::endl;
	}

	avr->frequency = F_CPU;
	avr_init(avr);
	avr_load_firmware(avr, fw);

	void *leds = init_leds(avr);

	printf("firmware loaded for %s at %d Hz\n", fw->mmcu, avr->frequency);
	fflush(stdout);

	std::cout << "ATtiny85 simulation started"
		<< std::endl
		<< "Initial PC: "
		<< avr->pc
		<< std::endl
		<< "Press any key to exit"
		<< std::endl;

	uint64_t cycle_count = 0;

	while (keep_running) {
		int state = avr_run(avr);

		if (state == cpu_Done || state == cpu_Crashed) {
			printf("\nSimulation ended: state=%d\n", state);
			break;
		}

		cycle_count++;

		if (cycle_count % 100 == 0) {
			printf("\rCycle count: %012lu", cycle_count);
			fflush(stdout);
		}

		if (kb()) {
			getchar();
			break;
		}
	}

	avr_terminate(avr);
	free(leds);
	free(fw);
	std::cout << std::endl << "ran for " << cycle_count << " cycles." << std::endl;
	return 0;
}

static void set_nb(int enable)
{
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

int main(int argc, const char **argv)
{
	int result = 0;
	signal(SIGINT, int_handler);
	set_nb(true);
	result = simulate();
	set_nb(false);
	return result;
}
