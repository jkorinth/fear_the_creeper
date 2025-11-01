.PHONY: sim fmt
BUILD_DIR=./build
SRCS=$(wildcard src/*)
HDRS=$(wildcard inc/*)
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"

sim: $(BUILD_DIR)/sim
	$(BUILD_DIR)/sim

build/sim: CMakeLists.txt $(SRCS) $(HDRS)
	cmake $(CMAKE_OPTS) -B $(BUILD_DIR) -S . && cmake --build $(BUILD_DIR) -j

clean:
	@rm -rf $(BUILD_DIR)

fmt: $(SRCS) $(HDRS)
	clang-format -i --style=Google $(SRCS) $(HDRS)
