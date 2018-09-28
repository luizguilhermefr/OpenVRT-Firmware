#!/usr/bin/env bash
MAIN_FILE=main.ino
PORT=/dev/ttyUSB0
echo "Verifying and uploading..."
$ARDUINO_SDK_HOME/arduino --upload src/${MAIN_FILE} --port ${PORT} && echo "Done!"
