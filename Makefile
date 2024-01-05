PORT ?= /dev/ttyUSB0
BUILD_DIR ?= .build

ARDUINO_OUTS = sketch.ino.bin sketch.ino.bootloader.bin sketch.ino.elf sketch.ino.map sketch.ino.partitions.bin
ARDUINO_OUT_PATHS = $(addprefix $(BUILD_DIR)/arduino-out/,$(ARDUINO_OUTS))

.PHONY: all
all: build

.PHONY: build
build: build.sketch build.test build.example.cli

.PHONY: build.sketch
build.sketch: $(ARDUINO_OUT_PATHS)

.PHONY: build.example.cli
build.example.cli: $(BUILD_DIR)/examples/cli

.PHONY: build.test
build.test: $(BUILD_DIR)/test

.PHONY: install
install: $(addprefix $(out)/,$(ARDUINO_OUTS)) $(out)/bin/test

.PHONY: test
test: $(BUILD_DIR)/test
	$(BUILD_DIR)/test

$(out)/%: $(out)/arduino-out/%
	install -D $< $@

$(out)/bin/test: $(BUILD_DIR)/test
	install -D --mode 755 $< $@

$(BUILD_DIR)/test: $(BUILD_DIR)/munit/munit.o $(BUILD_DIR)/dcc.o $(BUILD_DIR)/test.o
	@mkdir -p $(@D)
	$(CC) -o $@ $^

$(BUILD_DIR)/test.o: test/main.c
	@mkdir -p $(@D)
	$(CC) -Wextra -Wno-unused-parameter -Ilib/munit -Isrc -c -o $@ $^

$(BUILD_DIR)/dcc.o: src/dcc.c
	@mkdir -p $(@D)
	$(CC) -Wextra -Wconversion -Wdeprecated -Isrc -c -o $@ $^

$(BUILD_DIR)/munit/munit.o: lib/munit/munit.c
	@mkdir -p $(@D)
	$(CC) -Ilib/munit -c -o $@ $^

$(ARDUINO_OUT_PATHS)&: sketch/sketch.ino sketch/sketch.yaml sketch/lv_conf.h
	arduino-cli\
	  compile\
	  --build-path $(BUILD_DIR)/arduino\
	  --build-cache-path $(BUILD_DIR)/arduino-cache\
	  --output-dir $(BUILD_DIR)/arduino-out\
	  --libraries .,lib\
	  sketch

$(BUILD_DIR)/examples/cli: $(BUILD_DIR)/examples/cli.o $(BUILD_DIR)/dcc.o
	@mkdir -p $(@D)
	$(CC) -o $@ $^

$(BUILD_DIR)/examples/cli.o: examples/cli/main.c
	@mkdir -p $(@D)
	$(CC) -Wextra -Wconversion -Wdeprecated -Isrc -c -o $@ $^

.PHONY: upload
upload: build
	arduino-cli upload --port $(PORT) --input-dir $(BUILD_DIR)/arduino-out sketch

.PHONY: format
format: format.c format.nix

.PHONY: format.c
format.c:
	clang-format --style=file -i $$(git ls-files | grep '.*\.\([ch]\|ino\)$$')

.PHONY: format.nix
format.nix:
	nixpkgs-fmt $$(git ls-files | grep '.*\.nix$$')

.PHONY: setup
setup:
	# first `arduino-cli config init`
	arduino-cli config add board_manager.additional_urls https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/arduino/package_m5stack_index.json
	arduino-cli core install m5stack:esp32
	arduino-cli lib install M5Stack M5GFX

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) debug_custom.json debug.cfg esp32.svd
