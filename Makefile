CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -O2
TARGET=energy_efficiency
SOURCE=energy_efficiency.c

.PHONY: all clean install test

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

test: $(TARGET)
	@echo "=== Testing Energy Efficiency Program ==="
	@echo "Current CPU governor before:"
	-@cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor 2>/dev/null || echo "N/A"
	@echo "Current CPU frequency before:"
	-@cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq 2>/dev/null || echo "N/A"
	@echo ""
	@echo "Running energy efficiency optimization..."
	sudo ./$(TARGET)
	@echo ""
	@echo "Current CPU governor after:"
	-@cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor 2>/dev/null || echo "N/A"
	@echo "Current CPU frequency after:"
	-@cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq 2>/dev/null || echo "N/A"

monitor:
	@echo "=== System Power Information ==="
	@echo "Available CPU frequencies:"
	-@cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies 2>/dev/null || echo "N/A"
	@echo "Available CPU governors:"
	-@cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors 2>/dev/null || echo "N/A"
	@echo "Current governor:"
	-@cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor 2>/dev/null || echo "N/A"
	@echo "Current frequency:"
	-@cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq 2>/dev/null || echo "N/A"
	@echo ""
	@echo "USB devices power control:"
	-@find /sys/bus/usb/devices/usb*/power/control -exec sh -c 'echo "$$1: $$(cat $$1 2>/dev/null || echo N/A)"' _ {} \; 2>/dev/null

reset:
	@echo "Resetting CPU governor to ondemand..."
	-@echo 'ondemand' | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor >/dev/null 2>&1
	@echo "Reset complete."
