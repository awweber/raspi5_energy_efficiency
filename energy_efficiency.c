/*Aufgabe: Optimierung der Energieeffizienz eines Embedded Linux-Systems

In dieser Übungsaufgabe sollst du die Energieeffizienz eines Embedded Linux-Systems optimieren, indem du die 
CPU-Frequenz dynamisch anpasst und nicht benötigte Peripheriegeräte deaktivierst. Du verwendest das cpufreq-Subsystem 
und das Runtime Power Management (pm_runtime) von Linux, um diese Aufgaben zu erfüllen.

Der folgende C-Code für eine einfache Anwendung enthält Platzhalter für die Implementierung der Energieeffizienzmaßnahmen. 
Deine Aufgabe besteht darin, diese Maßnahmen zu implementieren und den Energieverbrauch des Systems zu reduzieren.

Anforderung:
- Implementiere die Funktion set_cpu_frequency, um die CPU-Frequenz dynamisch anzupassen.
- Implementiere die Funktion disable_unused_peripherals, um nicht benötigte Peripheriegeräte zu deaktivieren.
- Kompiliere die Anwendung und führe sie auf deinem Embedded Linux-System aus.
- Überprüfe den Energieverbrauch vor und nach der Optimierung, um die Auswirkungen deiner Änderungen zu bewerten.

1: Implementierung der Funktion set_cpu_frequency
Implementiere die Funktion set_cpu_frequency, um die CPU-Frequenz dynamisch anzupassen.

2: Implementierung der Funktion disable_unused_peripherals
Implementiere die Funktion disable_unused_peripherals, um nicht benötigte Peripheriegeräte zu deaktivieren.

3: Kompilieren und Ausführen der Anwendung
- Kompiliere die Anwendung mit GCC.
bash: gcc -o energy_efficiency energy_efficiency.c

Führe die Anwendung auf deinem Embedded Linux-System aus.
bash: ./energy_efficiency

4: Überprüfung des Energieverbrauchs
Überprüfe den Energieverbrauch vor und nach der Optimierung, um die Auswirkungen deiner Änderungen zu bewerten. 
Verwende dazu Werkzeuge wie PowerTop oder einen Monsoon Power Monitor.

bash: sudo powertop
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// Hilfsfunktion zum Schreiben in sysfs-Dateien
int write_to_sysfs(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        printf("Warning: Failed to open %s: %s\n", path, strerror(errno));
        return -1;
    }
    
    if (write(fd, value, strlen(value)) == -1) {
        printf("Warning: Failed to write '%s' to %s: %s\n", value, path, strerror(errno));
        close(fd);
        return -1;
    }
    
    close(fd);
    printf("Successfully wrote '%s' to %s\n", value, path);
    return 0;
}

// Funktion zum Setzen des CPU-Governors
void set_cpu_governor(const char *governor) {
    char path[256];
    DIR *dir;
    struct dirent *entry;
    
    printf("Setting CPU governor to '%s'...\n", governor);
    
    // Iteriere über alle verfügbaren CPUs
    dir = opendir("/sys/devices/system/cpu");
    if (dir == NULL) {
        perror("Failed to open /sys/devices/system/cpu");
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "cpu", 3) == 0 && 
            entry->d_name[3] >= '0' && entry->d_name[3] <= '9') {
            
            snprintf(path, sizeof(path), 
                    "/sys/devices/system/cpu/%s/cpufreq/scaling_governor", 
                    entry->d_name);
            
            write_to_sysfs(path, governor);
        }
    }
    
    closedir(dir);
}

// Funktion zur Anpassung der CPU-Frequenz
void set_cpu_frequency(const char *freq) {
    char path[256];
    DIR *dir;
    struct dirent *entry;
    
    printf("Setting CPU frequency to %s Hz...\n", freq);
    
    // Zuerst Governor auf 'userspace' setzen für manuelle Frequenzkontrolle
    set_cpu_governor("userspace");
    
    // Iteriere über alle verfügbaren CPUs
    dir = opendir("/sys/devices/system/cpu");
    if (dir == NULL) {
        perror("Failed to open /sys/devices/system/cpu");
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "cpu", 3) == 0 && 
            entry->d_name[3] >= '0' && entry->d_name[3] <= '9') {
            
            snprintf(path, sizeof(path), 
                    "/sys/devices/system/cpu/%s/cpufreq/scaling_setspeed", 
                    entry->d_name);
            
            write_to_sysfs(path, freq);
        }
    }
    
    closedir(dir);
}
// Funktion zur Deaktivierung nicht benötigter Peripheriegeräte
void disable_unused_peripherals() {
    printf("Disabling unused peripheral devices...\n");
    
    // USB-Geräte in den Auto-Suspend-Modus versetzen
    const char *usb_devices[] = {
        "/sys/bus/usb/devices/usb1/power/control",
        "/sys/bus/usb/devices/usb2/power/control",
        "/sys/bus/usb/devices/usb3/power/control",
        "/sys/bus/usb/devices/usb4/power/control"
    };
    
    for (int i = 0; i < sizeof(usb_devices) / sizeof(usb_devices[0]); i++) {
        write_to_sysfs(usb_devices[i], "auto");
    }
    
    // Bluetooth deaktivieren (falls vorhanden)
    write_to_sysfs("/sys/class/rfkill/rfkill0/state", "0");
    
    // WiFi Power Management aktivieren (falls vorhanden)
    write_to_sysfs("/sys/class/net/wlan0/power/control", "auto");
    
    // Audio-Geräte in Power-Save-Modus versetzen
    write_to_sysfs("/sys/module/snd_hda_intel/parameters/power_save", "1");
    
    // Festplatten-Power-Management aktivieren
    const char *storage_devices[] = {
        "/sys/block/sda/queue/rotational",
        "/sys/block/sdb/queue/rotational"
    };
    
    for (int i = 0; i < sizeof(storage_devices) / sizeof(storage_devices[0]); i++) {
        write_to_sysfs(storage_devices[i], "0");
    }
    
    // GPU Power Management aktivieren (falls Intel-Grafik vorhanden)
    write_to_sysfs("/sys/class/drm/card0/power/control", "auto");
    
    // Ethernet-Adapter Power Management
    write_to_sysfs("/sys/class/net/eth0/power/control", "auto");
    
    printf("Peripheral power management configuration completed.\n");
}
// Funktion zur Anzeige verfügbarer CPU-Frequenzen
void show_available_frequencies() {
    printf("Available CPU frequencies:\n");
    write_to_sysfs("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies", "");
}

// Funktion zur adaptiven Frequenzanpassung basierend auf Last
void adaptive_frequency_scaling() {
    printf("Enabling adaptive frequency scaling...\n");
    
    // Powersave-Governor für maximale Energieeffizienz
    set_cpu_governor("powersave");
    
    // Oder konservative Einstellung für bessere Balance
    // set_cpu_governor("conservative");
}

// Funktion zur Energieverbrauchsmessung (simuliert)
void monitor_power_consumption() {
    printf("Monitoring power consumption...\n");
    printf("Tip: Use 'sudo powertop' or 'cat /proc/cpuinfo' to check current frequencies\n");
    printf("Tip: Use 'cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq' for current CPU frequencies\n");
}

int main() {
    printf("=== Energy Efficiency Optimization for Embedded Linux ===\n\n");
    
    // Zeige verfügbare Informationen
    monitor_power_consumption();
    
    printf("\n1. Setting up power-efficient CPU governor...\n");
    adaptive_frequency_scaling();
    
    printf("\n2. Configuring manual CPU frequency (low power mode)...\n");
    // Setze eine niedrige Frequenz für maximale Energieeffizienz
    set_cpu_frequency("800000");  // 800 MHz
    
    printf("\n3. Disabling unused peripheral devices...\n");
    disable_unused_peripherals();
    
    printf("\n=== Energy efficiency optimization complete ===\n");
    printf("\nRecommendations:\n");
    printf("- Monitor with: sudo powertop\n");
    printf("- Check CPU freq: cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq\n");
    printf("- Reset governor: echo 'ondemand' | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor\n");
    
    return 0;
}