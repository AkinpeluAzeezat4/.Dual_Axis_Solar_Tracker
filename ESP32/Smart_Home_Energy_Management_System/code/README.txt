Smart Home Energy Management System - ESP32 PlatformIO Code

Local server:
- WiFi AP SSID: SHEMS-Controller
- Password: 12345678
- Browser: http://192.168.4.1

Boot settings entry:
- Hold the rotary encoder button during boot for about 3 seconds to enter settings.
- During normal operation, hold the encoder button for about 5 seconds to enter settings.

Main behavior:
- The controller runs autonomously after boot.
- Relay settings store the expected connected load power of each load.
- Inverter Power and System Power are set under System Setup.
- If load approaches/exceeds the inverter limit and PHCN is available, the controller keeps lower-power loads on inverter and moves higher-power/less-prioritized loads to PHCN.
- The load manager uses fuzzy risk evaluation and a small genetic-style optimizer over the 6 relay combinations.

Important pins kept from the uploaded schematic/project convention:
- ST7920 LCD: CS=GPIO5, SCK=GPIO18, MOSI=GPIO23
- Rotary encoder: A=GPIO26, B=GPIO25, SW=GPIO27
- Heartbeat LED: GPIO2

Relay output pins are centralized in lib/Pins/Pins.cpp. Adjust RELAY_1 to RELAY_6 there if your PCB uses different relay pins.
