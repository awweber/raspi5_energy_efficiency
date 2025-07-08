# Energy Efficiency Optimization for Embedded Linux

Dieses Projekt implementiert Energieeffizienz-Optimierungen für Embedded Linux-Systeme durch dynamische CPU-Frequenzanpassung und Power Management von Peripheriegeräten.

## Übersicht

Das Programm nutzt das Linux cpufreq-Subsystem und Runtime Power Management (pm_runtime), um:
- CPU-Frequenz dynamisch anzupassen
- Nicht benötigte Peripheriegeräte zu deaktivieren
- Energieverbrauch zu reduzieren

## Funktionen

### 1. CPU-Frequenz-Management
- **set_cpu_governor()**: Setzt den CPU-Governor (powersave, ondemand, userspace, etc.)
- **set_cpu_frequency()**: Setzt eine spezifische CPU-Frequenz
- **adaptive_frequency_scaling()**: Aktiviert adaptive Frequenzskalierung

### 2. Peripherie-Power-Management
- **disable_unused_peripherals()**: Deaktiviert oder versetzt Geräte in Power-Save-Modi:
  - USB-Geräte (Auto-Suspend)
  - Bluetooth
  - WiFi Power Management
  - Audio-Geräte
  - Festplatten-Power-Management
  - GPU Power Management
  - Ethernet-Adapter

### 3. Monitoring und Hilfsfunktionen
- **write_to_sysfs()**: Sichere sysfs-Schreiboperationen
- **monitor_power_consumption()**: Zeigt Power-Monitoring-Tipps

## Verwendung

### Kompilieren
```bash
# Mit Makefile
make

# Oder direkt mit gcc
gcc -Wall -Wextra -std=c99 -O2 -o energy_efficiency energy_efficiency.c
```

### Ausführen
```bash
# Das Programm benötigt Root-Rechte für sysfs-Zugriff
sudo ./energy_efficiency
```

### Testing
```bash
# Umfassender Test mit Benchmark
./test_energy.sh

# Oder einzelne Make-Targets
make test           # Test mit Vorher/Nachher-Vergleich
make monitor        # Zeige aktuelle Power-Informationen
make reset          # Setze CPU-Governor zurück
```

## CPU-Governors

- **powersave**: Minimale Frequenz für maximale Energieeffizienz
- **ondemand**: Dynamische Anpassung basierend auf Last (Standard)
- **conservative**: Sanftere Frequenzänderungen als ondemand
- **userspace**: Manuelle Frequenzkontrolle
- **performance**: Maximale Frequenz

## Sysfs-Pfade

### CPU-Frequenz-Kontrolle
- `/sys/devices/system/cpu/cpu*/cpufreq/scaling_governor`
- `/sys/devices/system/cpu/cpu*/cpufreq/scaling_setspeed`
- `/sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq`
- `/sys/devices/system/cpu/cpu*/cpufreq/scaling_available_frequencies`

### Power Management
- `/sys/bus/usb/devices/usb*/power/control`
- `/sys/class/rfkill/rfkill0/state` (Bluetooth)
- `/sys/class/net/wlan0/power/control` (WiFi)
- `/sys/module/snd_hda_intel/parameters/power_save` (Audio)
- `/sys/class/drm/card0/power/control` (GPU)

## Energieverbrauch messen

### PowerTOP
```bash
# Installation
sudo apt-get install powertop

# Ausführung
sudo powertop

# HTML-Report erstellen
sudo powertop --html=powertop_report.html
```

### CPU-Power-Tools
```bash
# Installation
sudo apt-get install linux-tools-generic

# CPU-Frequenz-Informationen
cpupower frequency-info

# Idle-States anzeigen
cpupower idle-info
```

### Weitere Monitoring-Tools
```bash
# Aktuelle CPU-Frequenz aller Kerne
cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq

# CPU-Load
cat /proc/loadavg

# Temperatur (falls verfügbar)
cat /sys/class/thermal/thermal_zone*/temp
```

## Optimierungsstrategien

### 1. CPU-Optimierung
- Verwende `powersave`-Governor für minimalen Verbrauch
- Setze niedrige maximale Frequenz bei konstanter Last
- Nutze `conservative`-Governor für Balance zwischen Performance und Effizienz

### 2. Peripherie-Optimierung
- Aktiviere Auto-Suspend für USB-Geräte
- Deaktiviere ungenutzte Funkschnittstellen (Bluetooth, WiFi)
- Konfiguriere aggressive Power-Management-Einstellungen

### 3. System-Level-Optimierungen
- Reduziere Kernel-Timer-Frequenz
- Optimiere I/O-Scheduler
- Konfiguriere CPU-Idle-States

## Troubleshooting

### Häufige Probleme

1. **Permission denied**: Programm mit `sudo` ausführen
2. **File not found**: Sysfs-Pfade können je nach System variieren
3. **Operation not supported**: Hardware unterstützt möglicherweise keine Frequenzskalierung

### Debugging
```bash
# Verfügbare Frequenzen prüfen
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies

# Verfügbare Governors prüfen
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors

# Kernel-Messages prüfen
dmesg | grep -i "cpufreq\|power"
```

## Sicherheitshinweise

- Zu niedrige CPU-Frequenzen können Systeminstabilität verursachen
- Backup der ursprünglichen Einstellungen vor Änderungen
- Teste Änderungen in einer sicheren Umgebung
- Überwache Systemtemperatur bei aggressiven Power-Management-Einstellungen

## Erweiterte Konfiguration

### Persistente Einstellungen
Um Einstellungen nach Neustart beizubehalten, erstelle systemd-Services oder verwende `/etc/rc.local`.

### Hardware-spezifische Anpassungen
- Intel-Systeme: Nutze `intel_pstate`-Driver
- ARM-Systeme: Konfiguriere `cpufreq-dt`
- Embedded-Systeme: Anpassung der Device-Tree-Einträge

## Lizenz

Dieses Projekt dient Bildungszwecken und kann frei verwendet und modifiziert werden.
