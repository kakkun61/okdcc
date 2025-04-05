PORT ?= /dev/ttyUSB0
BUILD_DIR ?= .build
ABS_BUILD_DIR = $(shell realpath $(BUILD_DIR))

OKDCC_LOGIC_SOURCES = $(shell find logic/src -type f -name '*.c')
OKDCC_ELECTRIC_SOURCES = $(shell find electric/src -type f -name '*.c')
OKDCC_UI_SOURCES = $(shell find ui/src -type f -name '*.c')
OKDCC_LOGIC_OBJECTS = $(patsubst logic/src/%,$(ABS_BUILD_DIR)/okdcc/logic/%,$(OKDCC_LOGIC_SOURCES:.c=.o))
OKDCC_ELECTRIC_OBJECTS = $(patsubst electric/src/%,$(ABS_BUILD_DIR)/okdcc/electric/%,$(OKDCC_ELECTRIC_SOURCES:.c=.o))
OKDCC_UI_MOCK_X11_OBJECTS = $(patsubst ui/src/%,$(ABS_BUILD_DIR)/okdcc/mock/x11.d/%,$(OKDCC_UI_SOURCES:.c=.o))

PLATFORMIO_ENVIRONMENT = debug
PLATFORMIO_OUT_EXTS = bin elf map
PLATFORMIO_OUTS = $(addprefix firmware.,$(PLATFORMIO_OUT_EXTS))
APP_MONITOR_OUT_DIR = $(ABS_BUILD_DIR)/okdcc/app/monitor/build
APP_MONITOR_OUT_PATHS = $(addprefix $(APP_MONITOR_OUT_DIR)/$(PLATFORMIO_ENVIRONMENT)/,$(PLATFORMIO_OUTS))
TEST_ELECTRIC_OUT_DIR = $(ABS_BUILD_DIR)/okdcc/test/electric/build
TEST_ELECTRIC_OUT_PATHS = $(addprefix $(TEST_ELECTRIC_OUT_DIR)/$(PLATFORMIO_ENVIRONMENT)/,$(PLATFORMIO_OUTS))

LVGL_DIR = lib/lvgl
LVGL_SOURCES = $(shell find $(LVGL_DIR)/src -type f -name '*.c' -not -path '*/\.*')
LVGL_MOCK_X11_OBJECTS = $(patsubst $(LVGL_DIR)/src/%,$(ABS_BUILD_DIR)/okdcc/mock/x11.d/lvgl/%,$(LVGL_SOURCES:.c=.o))

# spell-checker:ignore Wextra Wconversion Wdeprecated
CC_OPTS = -Wall -Wextra -Wconversion -Wdeprecated -Wno-unused-parameter -D DCC_ASSERT

.PHONY: all
all: build

.PHONY: build
build: build.logic build.app.monitor build.logic.test build.electric.test build.example.cli build.mock.x11

.PHONY: build.logic
build.logic: $(OKDCC_LOGIC_OBJECTS)

.PHONY: build.logic.test
build.logic.test: $(ABS_BUILD_DIR)/okdcc/logic/test/unit

.PHONY: build.app.monitor
build.app.monitor: $(APP_MONITOR_OUT_PATHS)

.PHONY: build.mock.x11
build.mock.x11: $(ABS_BUILD_DIR)/okdcc/mock/x11

.PHONY: build.example.cli
build.example.cli: $(ABS_BUILD_DIR)/okdcc/examples/cli

.PHONY: build.example.show
build.example.show: $(ABS_BUILD_DIR)/okdcc/examples/show

.PHONY: build.electric.test
build.electric.test: $(TEST_ELECTRIC_OUT_PATHS)

.PHONY: test
test: $(ABS_BUILD_DIR)/okdcc/logic/test/unit
	$(ABS_BUILD_DIR)/okdcc/logic/test/unit

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
doc: doc.en doc.ja

.PHONY: doc.en
doc.en:
	@mkdir -p $(ABS_BUILD_DIR)/okdcc/doc/doxygen/en
	(cd doc && doxygen)
	(cd doc && sphinx-build -M html . $(ABS_BUILD_DIR)/okdcc/doc/sphinx/en)

.PHONY: doc.ja
doc.ja: doc/locales/ja/html/LC_MESSAGES/index.mo
	@mkdir -p $(ABS_BUILD_DIR)/okdcc/doc/doxygen/ja
	(cd doc && doxygen Doxyfile.ja)
	(cd doc && sphinx-build -M html --define language=ja --define breathe_projects.okdcc=$(ABS_BUILD_DIR)/okdcc/doc/doxygen/ja/xml . $(ABS_BUILD_DIR)/okdcc/doc/sphinx/ja)

# On Read the Docs CI
.PHONY: doc.rtd
doc.rtd: doc/locales/ja/html/LC_MESSAGES/index.mo
	@mkdir -p "$(ABS_BUILD_DIR)/okdcc/doc/doxygen/$$READTHEDOCS_LANGUAGE"
	@mkdir -p "$$READTHEDOCS_OUTPUT/html"
	(cd doc && ln -s Doxyfile Doxyfile.en)
	(cd doc && doxygen "Doxyfile.$$READTHEDOCS_LANGUAGE")
	(cd doc && sphinx-build -M html --define "language=$$READTHEDOCS_LANGUAGE" --define "breathe_projects.okdcc=$(ABS_BUILD_DIR)/okdcc/doc/doxygen/$$READTHEDOCS_LANGUAGE/xml" . "$$READTHEDOCS_OUTPUT/html")

.PHONY: clean
clean:
	-$(RM) -r $(ABS_BUILD_DIR)

$(ABS_BUILD_DIR)/okdcc/logic/test/unit: $(ABS_BUILD_DIR)/munit/munit.o $(OKDCC_LOGIC_OBJECTS) $(ABS_BUILD_DIR)/okdcc/logic/test/unit.o
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -o $@ $^

$(ABS_BUILD_DIR)/okdcc/logic/test/unit.o: logic/test/unit/main.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I lib/munit -I logic/src -c -o $@ $^

$(ABS_BUILD_DIR)/okdcc/logic/%.o: logic/src/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I src -c -o $@ $^

$(ABS_BUILD_DIR)/okdcc/electric/%.o: electric/src/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I src -c -o $@ $^

$(ABS_BUILD_DIR)/munit/munit.o: lib/munit/munit.c
	@mkdir -p $(@D)
	$(CC) -I lib/munit -c -o $@ $^

$(APP_MONITOR_OUT_PATHS)&: app/monitor/src/main.cc app/monitor/src/lv_conf.h app/monitor/platformio.ini $(OKDCC_LOGIC_SOURCES) $(OKDCC_ELECTRIC_SOURCES) $(OKDCC_UI_SOURCES)
	pio run --project-dir app/monitor --environment $(PLATFORMIO_ENVIRONMENT)

$(TEST_ELECTRIC_OUT_PATHS)&: test/electric/src/main.cc test/electric/platformio.ini $(OKDCC_ELECTRIC_SOURCES)
	pio run --project-dir test/electric --environment $(PLATFORMIO_ENVIRONMENT)

$(ABS_BUILD_DIR)/okdcc/examples/%: $(ABS_BUILD_DIR)/okdcc/examples/%.o $(OKDCC_LOGIC_OBJECTS)
	@mkdir -p $(@D)
	$(CC) -o $@ $^

$(ABS_BUILD_DIR)/okdcc/examples/%.o: examples/%/main.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I logic/src -c -o $@ $^

$(ABS_BUILD_DIR)/okdcc/mock/x11.d/lvgl/%.o: $(LVGL_DIR)/src/%.c mock/x11/lv_conf.h
	@mkdir -p $(@D)
	$(CC) -I mock/x11 -DLV_CONF_INCLUDE_SIMPLE -c -o $@ $<

$(ABS_BUILD_DIR)/okdcc/mock/x11.d/okdcc/%.o: ui/src/okdcc/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I lib/lvgl -I mock/x11 -D LV_CONF_INCLUDE_SIMPLE -c -o $@ $<

$(ABS_BUILD_DIR)/okdcc/mock/x11: $(LVGL_MOCK_X11_OBJECTS) $(OKDCC_UI_MOCK_X11_OBJECTS) mock/x11/main.c
	@mkdir -p $(@D)
	$(CC) $(CC_OPTS) -I lib/lvgl -I mock/x11 -I ui/src -l X11 -l pthread -l m -D LV_CONF_INCLUDE_SIMPLE -o $@ $^

# spellchecker:ignore msgfmt
doc/locales/ja/html/LC_MESSAGES/index.mo: doc/locales/ja/html/LC_MESSAGES/index.po
	@mkdir -p $(@D)
	msgfmt -o $@ $<

# spellchecker:ignore msgmerge
doc/locales/ja/html/LC_MESSAGES/index.po: $(ABS_BUILD_DIR)/okdcc/doc/sphinx/gettext/index.pot
	@mkdir -p $(@D)
	if [ -f $@ ]; then msgmerge --update $@ $<; else cp $< $@; fi

$(ABS_BUILD_DIR)/okdcc/doc/sphinx/gettext/index.pot: doc/conf.py doc/index.rst
	(cd doc && sphinx-build -M gettext . $(ABS_BUILD_DIR)/okdcc/doc/sphinx)
