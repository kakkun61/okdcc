cmake_minimum_required(VERSION 3.16.0)

# M5Unified と M5GFX は Nix の devShell が OKDCC_ESP_COMPONENTS に用意する
set(EXTRA_COMPONENT_DIRS $ENV{OKDCC_ESP_COMPONENTS})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(electric)
