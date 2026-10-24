cmake_minimum_required(VERSION 3.16.0)

# M5Unified と M5GFX は Nix の devShell が OKDCC_ESP_COMPONENTS に用意する
set(EXTRA_COMPONENT_DIRS
  $ENV{OKDCC_ESP_COMPONENTS}
  ${CMAKE_CURRENT_LIST_DIR}/../../lib/lvgl)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# LVGL の設定は Kconfig ではなく main/lv_conf.h で行う
idf_build_set_property(COMPILE_DEFINITIONS LV_KCONFIG_IGNORE APPEND)
idf_build_set_property(COMPILE_DEFINITIONS LV_CONF_INCLUDE_SIMPLE APPEND)
idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/main APPEND)

project(monitor)
