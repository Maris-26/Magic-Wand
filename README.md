# TECHIN515 Magic Wand Project

## Overview

This project is a gesture-recognition-based magic wand built with an ESP32 and an MPU6050 sensor. The wand detects specific motion patterns and classifies them using a model trained in Edge Impulse. Based on the result, it lights up an RGB LED to simulate "spells."

## Folder Structure

```
TECHIN515-magic-wand/
├── src/
│   ├── sketches/                 # Arduino sketches with comments and Edge Impulse exports
│   ├── python-scripts/           # Python scripts for data collection
│   │   └── dataset/              # Collected and labeled gesture data
├── docs/
│   └── report.pdf                # Final project report in PDF format
├── media/
│   └── demo.mp4                  # Demo video file or link
├── enclosure/
│   ├── final-enclosure-images/  # Images of the 3D-printed wand housing
│   └── notes.md                 # Design rationale, materials, battery placement, etc.
├── README.md                    # This file
└── .gitignore                   # Ignore compiled/intermediate and temp files
```

## Setup Instructions

1. **Hardware**:

   * Connect MPU6050 to ESP32 I2C (SDA/SCL).
   * Wire RGB LED to digital pins (e.g., RED: 21, GREEN: 20, BLUE: 8).
   * Use onboard button (or jumper wire) for gesture capture trigger.

2. **Software Requirements**:

   * Arduino IDE (v2.x or later)
   * Edge Impulse CLI (for uploading/exporting models)
   * Python 3 (for data collection scripts)

3. **Steps to Run**:

   * Upload `gesture_capture.ino` to ESP32 to collect gesture data.
   * Run `process_gesture_data.py` to organize collected data.
   * Train model using Edge Impulse with `Spectral Features + NN Classifier`.
   * Export C++ model and include it in `wand.ino`.
   * Upload `wand.ino` to ESP32 and perform live gesture classification.

## Features

* Real-time gesture recognition (O, V, Z)
* RGB LED feedback for visual response
* Trained and deployed via Edge Impulse
* Lightweight model runs directly on ESP32-C3

## Demo

See `media/demo.mp4` for a demonstration of the magic wand in action.

## Notes

This project required debugging LED pin issues and model misclassification. The final model achieved 90% training accuracy and 80% validation accuracy with minimal memory footprint.

## License

Apache 2.0 (for Edge Impulse SDK components). All other project files are open for educational use.
