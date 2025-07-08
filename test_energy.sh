#!/bin/bash

# Energy Efficiency Testing Script for Embedded Linux
# This script helps test and benchmark the energy optimization

echo "=== Energy Efficiency Testing Script ==="
echo "Date: $(date)"
echo "System: $(uname -a)"
echo ""

# Function to check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then
        echo "Note: Some operations require root privileges."
        echo "Run with sudo for full functionality."
        echo ""
    fi
}

# Function to display current power state
show_power_state() {
    echo "=== Current Power State ==="
    
    echo "CPU Information:"
    echo "- Governor: $(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor 2>/dev/null || echo 'N/A')"
    echo "- Current Freq: $(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq 2>/dev/null || echo 'N/A') Hz"
    echo "- Min Freq: $(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq 2>/dev/null || echo 'N/A') Hz"
    echo "- Max Freq: $(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq 2>/dev/null || echo 'N/A') Hz"
    
    echo ""
    echo "Available Governors:"
    cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors 2>/dev/null || echo "N/A"
    
    echo ""
    echo "Available Frequencies:"
    cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies 2>/dev/null || echo "N/A"
    
    echo ""
}

# Function to run power monitoring tools
monitor_power() {
    echo "=== Power Monitoring ==="
    
    if command -v powertop >/dev/null 2>&1; then
        echo "PowerTOP is available. Run 'sudo powertop' for detailed analysis."
    else
        echo "PowerTOP not found. Install with: sudo apt-get install powertop"
    fi
    
    if command -v cpupower >/dev/null 2>&1; then
        echo "CPU Power utilities available."
        echo "Current CPU info:"
        cpupower frequency-info 2>/dev/null | head -20 || echo "Run as root for detailed info"
    else
        echo "cpupower not found. Install with: sudo apt-get install linux-tools-generic"
    fi
    
    echo ""
}

# Function to benchmark
benchmark() {
    echo "=== Simple CPU Load Test ==="
    echo "Running CPU stress test for 10 seconds..."
    
    # Simple CPU load generator
    for i in {1..4}; do
        (
            end_time=$(($(date +%s) + 10))
            while [ $(date +%s) -lt $end_time ]; do
                : # busy loop
            done
        ) &
    done
    
    # Monitor frequency during load
    for i in {1..10}; do
        freq=$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq 2>/dev/null || echo "N/A")
        echo "Time: ${i}s, CPU Freq: $freq Hz"
        sleep 1
    done
    
    # Kill background processes
    pkill -f "bash.*benchmark" 2>/dev/null
    wait 2>/dev/null
    
    echo "Benchmark complete."
    echo ""
}

# Main script
check_root

echo "1. Showing current power state..."
show_power_state

echo "2. Compiling energy efficiency program..."
if make clean && make; then
    echo "Compilation successful."
else
    echo "Compilation failed. Check for errors."
    exit 1
fi

echo ""
echo "3. Testing power monitoring tools..."
monitor_power

echo "4. Running energy efficiency optimization..."
if [ "$EUID" -eq 0 ]; then
    ./energy_efficiency
else
    echo "Running with sudo for system modifications..."
    sudo ./energy_efficiency
fi

echo ""
echo "5. Showing power state after optimization..."
show_power_state

echo "6. Running benchmark to test frequency scaling..."
benchmark

echo "=== Testing Complete ==="
echo ""
echo "Recommendations:"
echo "- Compare CPU frequencies before and after optimization"
echo "- Use 'sudo powertop' for detailed power analysis"
echo "- Monitor power consumption with hardware tools if available"
echo "- Reset governor with: make reset"
echo ""
