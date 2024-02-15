PORT ?= /dev/ttyUSB0
BUILD_DIR ?= .build

ARDUINO_OUTS = sketch.ino.bin sketch.ino.bootloader.bin sketch.ino.elf sketch.ino.map sketch.ino.partitions.bin
ARDUINO_OUT_PATHS = $(addprefix $(BUILD_DIR)/arduino-out/,$(ARDUINO_OUTS))
ARDUINO_COMPILE_EXTRA_FLAGS = -DLV_CONF_INCLUDE_SIMPLE -Isketch

LVGL_DIR = lib/lvgl
LVGL_SOURCES = $(shell find $(LVGL_DIR)/src -type f -name '*.c' -not -path '*/\.*')
LVGL_MOCK_UI_X11_OBJECTS = $(patsubst $(LVGL_DIR)/src/%,$(BUILD_DIR)/mock/ui/x11/lvgl/%,$(LVGL_SOURCES:.c=.o))

CC_OPTS = -Wall -Wextra -Wconversion -Wdeprecated -Wno-unused-parameter

.PHONY: all
all: build

.PHONY: build
build: build.sketch build.test build.example.cli build.ui.mock.x11

.PHONY: build.sketch
build.sketch: $(ARDUINO_OUT_PATHS)

.PHONY: build.ui.mock.x11
build.ui.mock.x11: $(BUILD_DIR)/ui/mock/x11

.PHONY: build.example.cli
build.example.cli: $(BUILD_DIR)/examples/cli

.PHONY: build.test
build.test: $(BUILD_DIR)/test/unit

.PHONY: install
install: $(addprefix $(out)/,$(ARDUINO_OUTS)) $(out)/bin/test

.PHONY: test
test: $(BUILD_DIR)/test/unit
	$(BUILD_DIR)/test/unit

.PHONY: upload
upload: build.sketch
	arduino-cli --config-file ./arduino-cli.yaml upload --port $(PORT) --input-dir $(BUILD_DIR)/arduino-out sketch

.PHONY: format
format: format.c format.nix

.PHONY: format.c
format.c:
	clang-format --style=file -i $$(git ls-files | grep '.*\.\([ch]\|ino\)$$' | grep --invert-match '.*icon.*')

.PHONY: format.nix
format.nix:
	nixpkgs-fmt $$(git ls-files | grep '.*\.nix$$')

.PHONY: setup
setup:
	arduino-cli --config-file ./arduino-cli.yaml core install esp32:esp32
	arduino-cli --config-file ./arduino-cli.yaml lib install M5Stack M5GFX
# ↓ のエラーが出る。謎。ビルドはできる。
# Error initializing instance: Loading index file: loading json index file .build/arduino-data/package_m5stack_index.json: open .build/arduino-data/package_m5stack_index.json: no such file or directory

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR)

$(out)/%: $(out)/arduino-out/%
	install -D $< $@

$(out)/bin/test-unit: $(BUILD_DIR)/test/unit
	install -D --mode 755 $< $@

$(BUILD_DIR)/test/unit: $(BUILD_DIR)/munit/munit.o $(BUILD_DIR)/dcc.o $(BUILD_DIR)/test/unit.o
	@mkdir -p $(@D)
	$(CC) -o $@ $^

$(BUILD_DIR)/test/unit.o: test/unit/main.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -Ilib/munit -Isrc -c -o $@ $^

$(BUILD_DIR)/dcc.o: src/dcc.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -Isrc -c -o $@ $^

$(BUILD_DIR)/munit/munit.o: lib/munit/munit.c
	@mkdir -p $(@D)
	$(CC) -Ilib/munit -c -o $@ $^

$(ARDUINO_OUT_PATHS)&: sketch/sketch.ino sketch/sketch.yaml sketch/lv_conf.h src/dcc.c src/dcc.h src/okdcc.h src/ui.c src/ui.h
	arduino-cli\
	  --config-file ./arduino-cli.yaml\
	  compile\
	  --build-path $(BUILD_DIR)/arduino\
	  --build-cache-path $(BUILD_DIR)/arduino-cache\
	  --output-dir $(BUILD_DIR)/arduino-out\
	  --libraries lib\
	  --library .\
	  --build-property compiler.c.extra_flags="$(ARDUINO_COMPILE_EXTRA_FLAGS)"\
	  --build-property compiler.cpp.extra_flags="$(ARDUINO_COMPILE_EXTRA_FLAGS)"\
	  --build-property compiler.S.extra_flags="$(ARDUINO_COMPILE_EXTRA_FLAGS)"\
	  sketch

$(BUILD_DIR)/examples/cli: $(BUILD_DIR)/examples/cli.o $(BUILD_DIR)/dcc.o
	@mkdir -p $(@D)
	$(CC) -o $@ $^

$(BUILD_DIR)/examples/cli.o: examples/cli/main.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -Isrc -c -o $@ $^

$(BUILD_DIR)/mock/ui/x11/lvgl/%.o: $(LVGL_DIR)/src/%.c mock/ui/x11/lv_conf.h
	@mkdir -p $(@D)
	$(CC) -Imock/ui/x11 -DLV_CONF_INCLUDE_SIMPLE -c -o $@ $<

$(BUILD_DIR)/mock/ui/x11/ui.o: src/ui.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -Ilib/lvgl -Imock/ui/x11 -DLV_CONF_INCLUDE_SIMPLE -c -o $@ $<

$(BUILD_DIR)/ui/mock/x11: $(LVGL_MOCK_UI_X11_OBJECTS) $(BUILD_DIR)/mock/ui/x11/ui.o mock/ui/x11/main.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -Ilib/lvgl -Imock/ui/x11 -Isrc -lX11 -lpthread -lm -DLV_CONF_INCLUDE_SIMPLE -o $@ $^
