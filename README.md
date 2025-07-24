# BLE Soil Moisture Monitoring System

![System Diagram](https://i.imgur.com/JK7w3Yl.png)

A complete soil moisture monitoring system using BLE with:
- Web interface (GitHub Page)
- ESP32 Hub (central collector)
- ESP32 Sensors (data nodes)

## Live Demo

[View the web interface](https://cuilonger.github.io/BLE-Soil-Monitor)

## Features

- Real-time soil moisture monitoring
- Web-based dashboard with visual indicators
- BLE communication between sensors and hub
- Remote enable/disable functionality

## Hardware Requirements

- 1x ESP32 board (for hub)
- 1+ ESP32 boards (for sensors)
- Soil moisture sensors (for real deployment)

## Setup Instructions

1. Flash the hub firmware to your central ESP32
2. Flash the sensor firmware to your sensor ESP32(s) (change SENSOR_ID for each)
3. Open the web interface on Chrome/Edge
4. Connect to your hub via BLE

## Repository Structure

- `index.html` - Web interface (GitHub Page)
- `firmware/hub/` - Hub firmware (central collector)
- `firmware/sensor/` - Sensor firmware (data nodes)

## Browser Support

This application requires:
- Chrome/Edge browser
- Web Bluetooth support
- HTTPS connection (provided by GitHub Pages)

## License

MIT License - Free for personal and commercial use
