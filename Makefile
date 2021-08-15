PROJECT_NAME     := BLE_COMPUTER_SWITCH
TARGETS          := ble_computer_switch
OUTPUT_DIRECTORY := _build

APP_VERSION := 1
BOOTLOADER_VERSION := 1

SDK_ROOT := $(NRF5_SDK_PATH)
PROJ_DIR := .

PACK_NAME := app.hex

$(OUTPUT_DIRECTORY)/ble_computer_switch.out: \
  LINKER_SCRIPT  := ble_computer_switch.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs/ble_lbs.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_svci.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu_bonded.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu_unbonded.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_cts_c/ble_cts_c.c \
  $(PROJ_DIR)/main.c \
  $(PROJ_DIR)/ble_base.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_ppi.c \
  $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_wdt.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_ppi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_saadc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_timer.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spim.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi.c \
  $(SDK_ROOT)/components/libraries/led_softblink/led_softblink.c \
  $(SDK_ROOT)/components/libraries/low_power_pwm/low_power_pwm.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/sensorsim/sensorsim.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gq/nrf_ble_gq.c \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
  $(SDK_ROOT)/components/ble/ble_db_discovery/ble_db_discovery.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager_handler.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
  $(SDK_ROOT)/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas/ble_bas.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_twi.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_spi.c \

  

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR) \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/components/serialization/common \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/low_power_pwm \
  $(SDK_ROOT)/components/libraries/led_softblink \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/libraries/sensorsim \
  $(SDK_ROOT)/components/libraries/sortlist \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/libraries/atomic_flags \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/bootloader \
  $(SDK_ROOT)/components/libraries/bootloader/ble_dfu \
  $(SDK_ROOT)/components/libraries/bootloader/dfu \
  $(SDK_ROOT)/components/libraries/svc \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/sortlist \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/nrf_ble_gq \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/ble/ble_db_discovery \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/components/ble/ble_services/ble_cts_c \
  $(SDK_ROOT)/components/ble/ble_link_ctx_manager \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs \


# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -O0 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS := 
CFLAGS += $(OPT)
CFLAGS += -DAPP_TIMER_V2 
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED 
# CFLAGS += -DCONFIG_GPIO_AS_PINRESET  # 不使用P0.21作为复位引脚
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS  # 将NCF引脚（P0.09、P0.10）作为普通GPIO使用
CFLAGS += -DFLOAT_ABI_HARD 
CFLAGS += -DINITIALIZE_USER_SECTIONS 
CFLAGS += -DNO_VTOR_CONFIG 
CFLAGS += -DNRF52 
CFLAGS += -DNRF52832_XXAA 
CFLAGS += -DNRF52_PAN_74 
CFLAGS += -DNRF_SD_BLE_API_VERSION=7 
CFLAGS += -DBL_SETTINGS_ACCESS_ONLY 
CFLAGS += -DNRF_DFU_SVCI_ENABLED
CFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
CFLAGS += -DS132 
CFLAGS += -DSOFTDEVICE_PRESENT 
CFLAGS += -DCUSTOM_BOARD_INC=board
CFLAGS += -DAPP_VERSION=$(APP_VERSION)
CFLAGS += -DDEBUG
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
# CFLAGS += -Wall -Werror  # defined but not used
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

ble_computer_switch: CFLAGS += -D__HEAP_SIZE=8192
ble_computer_switch: CFLAGS += -D__STACK_SIZE=8192
ble_computer_switch: ASMFLAGS += -D__HEAP_SIZE=8192
ble_computer_switch: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm

ifdef TESTING
  PACK_NAME := testing_app.hex

  SRC_FILES += $(shell find $(PROJ_DIR)/tests -name "test*.c")

  INC_FOLDERS += $(PROJ_DIR)/tests

  CFLAGS += -DTESTING
endif


.PHONY: default help

# Default target - first one defined
default: ble_computer_switch

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		ble_computer_switch
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY:all flash_all flash flash_softdevice erase

flash_all:
	# nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/$(PACK_NAME) --sectorerase
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/$(PACK_NAME) --sectorerase
	nrfjprog -f nrf52 --reset

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/ble_computer_switch.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/ble_computer_switch.hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: s132_nrf52_7.0.1_softdevice.hex
	nrfjprog -f nrf52 --program $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_7.0.1_softdevice.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := $(PROJ_DIR)/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)