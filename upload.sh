#!/usr/bin/env bash
ARDUINO_SDK_PATH=/opt/arduino-1.8.6
MAIN_FILE=main.ino
PORT=/dev/ttyACM0
echo "Verifying and uploading..."
${ARDUINO_SDK_PATH}/arduino --upload ${MAIN_FILE} --port ${PORT} && echo "Done!"
