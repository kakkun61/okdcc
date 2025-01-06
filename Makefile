PORT ?= /dev/ttyUSB0
BUILD_DIR ?= .build

PLATFORMIO_ENVIRONMENT = debug
PLATFORMIO_OUT_EXTS = bin elf map
PLATFORMIO_OUTS = $(addprefix firmware.,$(PLATFORMIO_OUT_EXTS))
APP_MONITOR_PLATFORMIO_OUT_DIR = $(BUILD_DIR)/monitor/build
APP_MONITOR_OUT_PATHS = $(addprefix $(APP_MONITOR_PLATFORMIO_OUT_DIR)/$(PLATFORMIO_ENVIRONMENT)/,$(PLATFORMIO_OUTS))

LVGL_DIR = lib/lvgl
LVGL_SOURCES = $(shell find $(LVGL_DIR)/src -type f -name '*.c' -not -path '*/\.*')
LVGL_MOCK_UI_X11_OBJECTS = $(patsubst $(LVGL_DIR)/src/%,$(BUILD_DIR)/mock/ui/x11/lvgl/%,$(LVGL_SOURCES:.c=.o))

CC_OPTS = -Wall -Wextra -Wconversion -Wdeprecated -Wno-unused-parameter

.PHONY: all
all: build

.PHONY: build
build: build.app.monitor build.test build.example.cli build.ui.mock.x11

.PHONY: build.app.monitor
build.app.monitor: $(APP_MONITOR_OUT_PATHS)

.PHONY: build.ui.mock.x11
build.ui.mock.x11: $(BUILD_DIR)/ui/mock/x11

.PHONY: build.example.cli
build.example.cli: $(BUILD_DIR)/examples/cli

.PHONY: build.test
build.test: $(BUILD_DIR)/test/unit

.PHONY: test
test: $(BUILD_DIR)/test/unit
	$(BUILD_DIR)/test/unit

.PHONY: upload
upload: build.sketch
	pio run --project-dir app/monitor --environment $(PLATFORMIO_ENVIRONMENT) --target upload --upload-port $(PORT)

.PHONY: format
format: format.c format.nix

.PHONY: format.c
format.c:
	clang-format --style=file -i $$(git ls-files | grep '.*\.[ch]$$' | grep --invert-match '.*icon.*')

.PHONY: format.nix
format.nix:
	nixpkgs-fmt $$(git ls-files | grep '.*\.nix$$')

.PHONY: clean
clean:
	-$(RM) -r $(BUILD_DIR)

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

$(APP_MONITOR_OUT_PATHS)&: app/monitor/src/main.cc app/monitor/src/lv_conf.h app/monitor/platformio.ini src/dcc.c src/dcc.h src/okdcc.h src/ui.c src/ui.h
	pio run --project-dir app/monitor --environment $(PLATFORMIO_ENVIRONMENT)

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
