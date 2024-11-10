ARDUINO_CLI = arduino-cli
BOARD = rp2040:rp2040:adafruit_feather_can
LIBRARY = "Adafruit MCP2515"
SRC_DIR = src
BUILD_DIR = build

all: setup compile upload

setup:
	@echo "Setting up Arduino CLI and installing necessary tools..."
	$(ARDUINO_CLI) config add board_manager.additional_urls https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
	$(ARDUINO_CLI) core install rp2040:rp2040
	$(ARDUINO_CLI) lib install $(LIBRARY)

compile:
	@echo "Compiling the project..."
	$(ARDUINO_CLI) compile -b $(BOARD)

upload:
	@echo "Uploading to device..."
	$(ARDUINO_CLI) compile -b $(BOARD) --upload -o $(BUILD_DIR) $(SRC_DIR)

clean:
	@echo "Cleaning up build directory..."
	rm -rf $(BUILD_DIR)

help:
	@echo "Makefile for RP2040 Feather CAN Bus"
	@echo "Usage:"
	@echo "  make setup    - Set up the environment (install dependencies)"
	@echo "  make compile  - Compile the project"
	@echo "  make upload   - Upload the compiled code to the board"
	@echo "  make clean    - Clean the build directory"
	@echo "  make help     - Show this help message"

