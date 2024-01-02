PORT=/dev/ttyUSB0

.PHONY: all
all: build test

.PHONY: build
build: .build/arduino-out/sketch.ino.bootloader.bin

.PHONY: test
test: .build/unit
	.build/unit

.PHONY: examples.cli
examples.cli: .build/examples/cli

.build/unit: .build/munit/munit.o .build/dcc.o .build/unit.o
	@mkdir -p $(@D)
	clang -o $@ $^

.build/unit.o: test/main.c
	@mkdir -p $(@D)
	clang -Wextra -Wno-unused-parameter -Ilib -Isrc -c -o $@ $^

.build/dcc.o: src/dcc.c
	@mkdir -p $(@D)
	clang -Wextra -Wconversion -Wdeprecated -Isrc -c -o $@ $^

.build/munit/munit.o: lib/munit/munit.c
	@mkdir -p $(@D)
	clang -Ilib -c -o $@ $^

.build/arduino-out/sketch.ino.bootloader.bin: sketch/sketch.ino
	arduino-cli\
	  compile\
	  --build-path .build/arduino\
	  --build-cache-path .build/arduino-cache\
	  --output-dir .build/arduino-out\
	  --libraries .,lib\
	  sketch

.build/examples/cli: .build/examples/cli.o .build/dcc.o
	@mkdir -p $(@D)
	clang -o $@ $^

.build/examples/cli.o: examples/cli/main.c
	@mkdir -p $(@D)
	clang -Wextra -Wconversion -Wdeprecated -Isrc -c -o $@ $^

.PHONY: upload
upload: build
	arduino-cli upload --port $(PORT) --input-dir .build/arduino-out sketch

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
	-rm -rf .build debug_custom.json debug.cfg esp32.svd
