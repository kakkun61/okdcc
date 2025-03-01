PORT ?= /dev/ttyUSB0
BUILD_DIR ?= .build

OKDCC_LOGIC_SOURCES = $(shell find logic/src -type f -name '*.c')
OKDCC_ELECTRIC_SOURCES = $(shell find electric/src -type f -name '*.c')
OKDCC_UI_SOURCES = $(shell find ui/src -type f -name '*.c')
OKDCC_LOGIC_OBJECTS = $(patsubst logic/src/%,$(BUILD_DIR)/okdcc/logic/%,$(OKDCC_LOGIC_SOURCES:.c=.o))
OKDCC_ELECTRIC_OBJECTS = $(patsubst electric/src/%,$(BUILD_DIR)/okdcc/electric/%,$(OKDCC_ELECTRIC_SOURCES:.c=.o))
OKDCC_UI_MOCK_X11_OBJECTS = $(patsubst ui/src/%,$(BUILD_DIR)/okdcc/mock/x11.d/%,$(OKDCC_UI_SOURCES:.c=.o))

PLATFORMIO_ENVIRONMENT = debug
PLATFORMIO_OUT_EXTS = bin elf map
PLATFORMIO_OUTS = $(addprefix firmware.,$(PLATFORMIO_OUT_EXTS))
APP_MONITOR_OUT_DIR = $(BUILD_DIR)/okdcc/app/monitor/build
APP_MONITOR_OUT_PATHS = $(addprefix $(APP_MONITOR_OUT_DIR)/$(PLATFORMIO_ENVIRONMENT)/,$(PLATFORMIO_OUTS))
TEST_ELECTRIC_OUT_DIR = $(BUILD_DIR)/okdcc/test/electric/build
TEST_ELECTRIC_OUT_PATHS = $(addprefix $(TEST_ELECTRIC_OUT_DIR)/$(PLATFORMIO_ENVIRONMENT)/,$(PLATFORMIO_OUTS))

LVGL_DIR = lib/lvgl
LVGL_SOURCES = $(shell find $(LVGL_DIR)/src -type f -name '*.c' -not -path '*/\.*')
LVGL_MOCK_X11_OBJECTS = $(patsubst $(LVGL_DIR)/src/%,$(BUILD_DIR)/okdcc/mock/x11.d/lvgl/%,$(LVGL_SOURCES:.c=.o))

# spell-checker:words Wextra Wconversion Wdeprecated
CC_OPTS = -Wall -Wextra -Wconversion -Wdeprecated -Wno-unused-parameter -D DCC_ASSERT

.PHONY: all
all: build

.PHONY: build
build: build.logic build.app.monitor build.logic.test build.electric.test build.example.cli build.mock.x11

.PHONY: build.logic
build.logic: $(OKDCC_LOGIC_OBJECTS)

.PHONY: build.logic.test
build.logic.test: $(BUILD_DIR)/logic/test/unit

.PHONY: build.app.monitor
build.app.monitor: $(APP_MONITOR_OUT_PATHS)

.PHONY: build.mock.x11
build.mock.x11: $(BUILD_DIR)/okdcc/mock/x11

.PHONY: build.example.cli
build.example.cli: $(BUILD_DIR)/examples/cli

.PHONY: build.electric.test
build.electric.test: $(TEST_ELECTRIC_OUT_PATHS)

.PHONY: test
test: $(BUILD_DIR)/logic/test/unit
	$(BUILD_DIR)/logic/test/unit

.PHONY: upload.app.monitor
upload.app.monitor: build.app.monitor
	pio run --project-dir app/monitor --environment $(PLATFORMIO_ENVIRONMENT) --target upload --upload-port $(PORT)

.PHONY: format
format: format.c format.nix

.PHONY: format.c
format.c:
	clang-format --style=file -i $$(git ls-files | grep -G '.*\.\(c\|cc\|h\)$$' | grep --invert-match '.*icon.*')

.PHONY: format.nix
format.nix:
	nixpkgs-fmt $$(git ls-files | grep '.*\.nix$$')

.PHONY: doc
doc:
	@mkdir -p $(BUILD_DIR)/doc
	(cd doc && doxygen)
	(cd doc && sphinx-build --builder html . ../$(BUILD_DIR)/doc/sphinx)

.PHONY: clean
clean:
	-$(RM) -r $(BUILD_DIR)

$(BUILD_DIR)/logic/test/unit: $(BUILD_DIR)/munit/munit.o $(OKDCC_LOGIC_OBJECTS) $(BUILD_DIR)/logic/test/unit.o
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -o $@ $^

$(BUILD_DIR)/logic/test/unit.o: logic/test/unit/main.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I lib/munit -I logic/src -c -o $@ $^

$(BUILD_DIR)/okdcc/logic/%.o: logic/src/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I src -c -o $@ $^

$(BUILD_DIR)/okdcc/electric/%.o: electric/src/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I src -c -o $@ $^

$(BUILD_DIR)/munit/munit.o: lib/munit/munit.c
	@mkdir -p $(@D)
	$(CC) -I lib/munit -c -o $@ $^

$(APP_MONITOR_OUT_PATHS)&: app/monitor/src/main.cc app/monitor/src/lv_conf.h app/monitor/platformio.ini $(OKDCC_LOGIC_SOURCES) $(OKDCC_ELECTRIC_SOURCES) $(OKDCC_UI_SOURCES)
	pio run --project-dir app/monitor --environment $(PLATFORMIO_ENVIRONMENT)

$(TEST_ELECTRIC_OUT_PATHS)&: test/electric/src/main.cc test/electric/platformio.ini $(OKDCC_ELECTRIC_SOURCES)
	pio run --project-dir test/electric --environment $(PLATFORMIO_ENVIRONMENT)

$(BUILD_DIR)/examples/cli: $(BUILD_DIR)/examples/cli.o $(OKDCC_LOGIC_OBJECTS)
	@mkdir -p $(@D)
	$(CC) -o $@ $^

$(BUILD_DIR)/examples/cli.o: examples/cli/main.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I logic/src -c -o $@ $^

$(BUILD_DIR)/okdcc/mock/x11.d/lvgl/%.o: $(LVGL_DIR)/src/%.c mock/x11/lv_conf.h
	@mkdir -p $(@D)
	$(CC) -I mock/x11 -DLV_CONF_INCLUDE_SIMPLE -c -o $@ $<

$(BUILD_DIR)/okdcc/mock/x11.d/okdcc/%.o: ui/src/okdcc/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I lib/lvgl -I mock/x11 -D LV_CONF_INCLUDE_SIMPLE -c -o $@ $<

$(BUILD_DIR)/okdcc/mock/x11: $(LVGL_MOCK_X11_OBJECTS) $(OKDCC_UI_MOCK_X11_OBJECTS) mock/x11/main.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I lib/lvgl -I mock/x11 -I ui/src -l X11 -l pthread -l m -D LV_CONF_INCLUDE_SIMPLE -o $@ $^
