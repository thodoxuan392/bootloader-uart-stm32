# Communication Protocol over UART ( or USB, BLE ,...)

[Encrypt](#encrypt-future-work)  
[Protocol Format](#protocol-format)

-   [Command](#command-android-to-controllerbox)
-   [Status](#status-from-controllerbox-to-phone)
-   [Event](#event-from-controllerbox-to-phone)
-   [ResultCode](#result-code)

## Encrypt (Future Work)

This feature will be implemented when this product go to mass production.

## Protocol Format

| START_BYTE (1 byte) | PROTOCOL_ID (1 byte) | DATA_LEN (1 byte) | DATA[0:DATA_LEN] (Maximum 255 bytes) | CHECK_SUM (2 bytes - Algorithms CRC-16/ARC) | STOP_BYTE (1 byte) |
| ------------------- | -------------------- | ----------------- | ------------------------------------ | ------------------------------------------- | ------------------ |
| 0x78                | XX                   | XX                | XX[0:DATA_LEN]                       | XX                                          | 0x79               |

1. Request
2. Response (Indicate Android/ControllerBox processed message)

## Command (Android to ControllerBox)

1.  **Reset (Protocol 0x01)**

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ---------------- | --------- |
        | 0x78 | 0x01 | 0 | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN |RESULT_CODE| CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | -------- | ---------------- | --------- |
        | 0x78 | 0x01 | 1 | ${ResultCode} |${computationByCrc16Arc} | 0x79 |

2.  **Request Board Information (Protocol 0x02)**

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ---------------- | --------- |
        | 0x78 | 0x02 | 0 | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN |RESULT_CODE | SERIAL_NUMBER[0:5]| FIRMWARE_VERSION[0:2] | BOARD_VERSION[0:2] | BOARD_TYPE[0:1]|CHECK_SUM | STOP_BYTE |
        | ---------- | -----------| -------- | -------- | -------- | -------- | -------- | ---------------- | --------- | --------- |
        | 0x78 | 0x02 | 15 | ${ResultCode} | ${serialNumber} | ${firmwareVersion} | ${boardVersion} | ${boardType} |${computationByCrc16Arc} | 0x79 |

    -   Serial Number
    -   Firmware Version: Major (Index 0), Minor (Index 1), Patch (Index 2)
    -   Board Version: Major (Index 0), Minor (Index 1), Patch (Index 2)
    -   Board Type (uint16_t, MSB): 0x0001 - BOARD_COLOR_MIXER

3.  **Get Setting (Protocol 0x03)**

    Android will send the setting to the ControllerBox including:

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | CHECK_SUM | STOP_BYTE |
        | ---------- | -------- | -------- |-------- | -------- |
        | 0x78 | 0x03 | 0 | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | SETTING | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- |-------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x03 | ${settingLen} + 2 | ${ResultCode}| ${pipelineIndex}| ${setting} | ${computationByCrc16Arc} | 0x79 |

    -   Setting Byte as below:

        | BYTE_NO                                     | LEN | DATA                                        | RANGE                               |
        | ------------------------------------------- | --- | ------------------------------------------- | ----------------------------------- |
        | 0                                           | 1   | Pipeline[0] - Pulse Coefficient             | 0 -> 255                            |
        | 1-2                                         | 2   | Pipeline[0] - Pulse per 1 milliseconds      | 0 -> 65535                          |
        | 3-4                                         | 2   | Pipeline[0] - Pulse per 0.1 milliseconds    | 0 -> 65535                          |
        | 5-6                                         | 2   | Pipeline[0] - Pulse per 0.01 milliseconds   | 0 -> 65535                          |
        | ...                                         | ... | ...                                         | ...                                 |
        | 7 x pipelineIdx                             | 1   | Pipeline[idx] - Pulse Coefficient           | 0 -> 255                            |
        | 7 x pipelineIdx + 1: 7 x pipelineIdx + 2    | 2   | Pipeline[idx] - Pulse per 1 milliseconds    | 0 -> 65535                          |
        | 7 x pipelineIdx + 3: 7 x pipelineIdx + 4    | 2   | Pipeline[idx] - Pulse per 0.1 milliseconds  | 0 -> 65535                          |
        | 7 x pipelineIdx + 5: 7 x pipelineIdx + 6    | 2   | Pipeline[idx] - Pulse per 0.01 milliseconds | 0 -> 65535                          |
        | 7 x pipelineIdx + 7                         | 1   | Close Door Angle                            | 0 -> 255                            |
        | 7 x pipelineIdx + 8                         | 1   | Open Door Angle                             | 0 -> 255                            |
        | 7 x pipelineIdx + 9 - 7 x pipelineIdx + 10  | 2   | T-on for Push Color Stepper                 | 0 -> 65535                          |
        | 7 x pipelineIdx + 11 - 7 x pipelineIdx + 12 | 2   | T-on for Mix Color Stepper                  | 0 -> 65535                          |
        | 7 x pipelineIdx + 13                        | 1   | Mixer Speed Low Level                       | 0 -> 100                            |
        | 7 x pipelineIdx + 14                        | 1   | Mixer Speed Medium Level                    | 0 -> 100                            |
        | 7 x pipelineIdx + 15                        | 1   | Mixer Speed High Level                      | 0 -> 100                            |
        | 7 x pipelineIdx + 16                        | 1   | Mixer Current Speed Level                   | 0 -> 2 (0: Low, 1: Medium, 2: High) |

4.  **Update Setting (Protocol 0x04)**

    Android will send the setting to the ControllerBox including:

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | SETTING | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- |-------- | -------- | ---------------- |
        | 0x78 | 0x04 | ${settingLen} | ${setting} | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x04 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    -   Setting same as Command **Get Setting**

5.  **Ping (Protocol 0x05)**

    Android will send the ping request to create connection to Controller Box.

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | -------- | ---------------- |
        | 0x78 | 0x05 | 0 | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x05 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

6.  **Change Serial Number (Protocol 0x06)**

    Android will request to send the default Serial Number of Controller Box.

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | SERIAL_NUMBER[0:5] |CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | -------- | -------- | ---------------- |
        | 0x78 | 0x06 | 6 | ${serialNumber} | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x06 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

7.  **Sync Time (Protocol 0x07)**

    Android will request to synchronize the date time from PC to Color Machine.

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | YEAR[1:0] | MONTH | DATE| HOUR | MINUTE| SECOND |CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | -------- | -------- | ---------- | ----------- | -------- | -------- | -------- | ---------------- |
        | 0x78 | 0x07 | 7 | ${year} | ${month} | ${date} | ${hour} | ${minute} | ${second} | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x07 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    -   Date Time:
        -   Year(uint16, MSB): e,g 2024
        -   Month(uint8): e.g: August 0x08
        -   Date (uint8): e.g: 8
        -   Hour (uint8): e.g: 8 (8AM)
        -   Minute (uint8): e.g: 59
        -   Second (uint8): e.g: 0

8.  **Change Color Volume (Protocol 0x10)**

    App will send the request to change Color Volume at Pipeline

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | PIPELINE_INDEX | VOLUME[3:0] | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | -------- | ---------------- |-------- | ---------------- |
        | 0x78 | 0x10 | 1 | ${pipeLineIndex} | ${volume}| ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x10 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    -   pipelineIndex: Index of Pipeline in Controller Box from 0 -> 16
    -   volume: Volume of Pipeline for each mix time.

    **Note**: Command will be suppressed if Device is being locked

9.  **Start/Stop/Pause/Resume Push Color(Protocol 0x11)**

    App will send the request to start the Color Mixer

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | CMD | AUTO_RELOAD | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | -------- |-------- | -------- | ---------------- |
        | 0x78 | 0x11 | 1 | ${cmd} | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x11 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    -   cmd: 0x00 - start, 0x01 - stop, 0x02 - pause, 0x03 - resume

    **Note**: Command will be suppressed if Device is being locked

10. **Start/Stop/Pause/Resume Mix Color(Protocol 0x12)**

    App will send the request to stop the Color Mixer

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN |CMD | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | -------- |-------- | -------- |
        | 0x78 | 0x12 | 1 | ${cmd} | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x12 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    -   cmd: 0x00 - start, 0x01 - stop, 0x02 - pause, 0x03 - resume

    **Note**: Command will be suppressed if Device is being locked

11. **Control IO (Protocol 0x13)**

    Android request Controller to control generic IO.

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | SELECT_MASK | LEVEL_MASK | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | -------- |-------- | -------- | ---------------- |
        | 0x78 | 0x13 | 2 | ${selectMask} | ${levelMask} | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x13 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    -   selectMask: Mask of IOs are selected to control
    -   levelMask: Mask of IOs level

    **Note**: Command will be suppressed if Device is being locked

12. **Calibration (Protocol 0x14)**

    Android request Controller to calibration device/sensor.

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | DEVICE_INDEX | CALIBRATION_DATA_COEFFICIENT[7:0] | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | -------- |-------- | -------- | ---------------- |
        | 0x78 | 0x14 | 9 | ${deviceIndex} | ${calibrationData} | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x14 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

13. **Door Control (Protocol 0x15)**

    Android request Controller to open Door.

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | CONTROL | CHECK_SUM | STOP_BYTE |
        | ---------- | -------- | -------- |-------- | -------- | ---------------- |
        | 0x78 | 0x15 | 1| ${control} | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x15 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    -   Control:
        -   0x00: Request to Close Door
        -   0x01: Request to Open Door

    **Note**: Command will be suppressed if Device is being locked

14. **Set Expire Time (Protocol 0x16)**

    Android request Controller to set expire time. If the current time reach to expire time, Controller will be locked.

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | KEY[15:0] | YEAR[1:0] | MONTH | DATE| HOUR | MINUTE| SECOND | CHECK_SUM | STOP_BYTE |
        | ---------- | -------- | -------- |-------- |-------- | -------- |-------- | -------- |-------- |-------- | -------- | ---------------- |
        | 0x78 | 0x16 | 23 | ${key}  | ${year} | ${month} | ${date} | ${hour} | ${minute} | ${second} |${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- | -------- | ------------- | ------------------------ | --------- |
        | 0x78 | 0x16 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    -   Key(MSB): Unique Key for each license
    -   Expired Date:
        -   Year(uint16, MSB): e,g 2024
        -   Month(uint8): e.g: August 0x08
        -   Date (uint8): e.g: 8
        -   Hour (uint8): e.g: 8 (8AM)
        -   Minute (uint8): e.g: 59
        -   Second (uint8): e.g: 0

15. **Get Expire Time (Protocol 0x17)**

    Android request to get current expire time.

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | CHECK_SUM | STOP_BYTE |
        | ---------- | -------- | -------- |-------- | -------- |
        | 0x78 | 0x17 | 0 | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE |YEAR[1:0] | MONTH | DATE| HOUR | MINUTE| SECOND | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- |----------- | -------- | ------- | ------------- | ----------- |----------- | -------- | ------- | ------------- | --------- |
        | 0x78 | 0x17 | 8 | ${ResultCode} | ${year} | ${month} | ${date} | ${hour} | ${minute} | ${second} | ${computationByCrc16Arc} | 0x79 |

    -   Expired Date:
        -   Year(uint16, MSB): e,g 2024
        -   Month(uint8): e.g: August 0x08
        -   Date (uint8): e.g: 8
        -   Hour (uint8): e.g: 8 (8AM)
        -   Minute (uint8): e.g: 59
        -   Second (uint8): e.g: 0

16. **Change Color Volume for all pipeline (Protocol 0x18)**

    Android request to change color volume for all pipeline

    -   Request:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | VOLUME | CHECK_SUM | STOP_BYTE |
        | ---------- | -------- | -------- |-------- |-------- | -------- |
        | 0x78 | 0x18 | 4 | ${volume} | ${computationByCrc16Arc} | 0x79 |
    -   Response:
        | START_BYTE | PROTOCOL_ID | DATA_LEN | RESULT_CODE | CHECK_SUM | STOP_BYTE |
        | ---------- | ----------- |----------- | ------- | ------------- | --------- |
        | 0x78 | 0x18 | 1 | ${ResultCode} | ${computationByCrc16Arc} | 0x79 |

    -   volume: Volume of Pipeline for each push time

## Status (From Controller Box to App)

1. **Device Error (Protocol 0x30)**

    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | ERROR_FLAG[3:0]| CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ----------- | -------- | ----------- |
      | 0x78 | 0x30 | 1 | ${errorFlag}| ${computationByCrc16Arc} | 0x79 |

    - Error Flag (32bits): Bit Mask
    - Bit 0 -> 16:
    - Bit 17: EEPROM Error

2. **Input Status (Protocol 0x31)**

    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | INPUT_STATUS | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ----------- | -------- | ----------- |
      | 0x78 | 0x31 | 1 | ${inputStatus}| ${computationByCrc16Arc} | 0x79 |

    - InputStatus: Bit Mask
    - Bit 0: Door Closed
    - Bit 1: Door Opened
    - Bit 2: CAN detected
    - Bit 3: Reserved

3. **Machine Status (Protocol 0x32)**

    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | MACHINE_STATUS | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | ----------- | -------- | ----------- |
      | 0x78 | 0x32 | 1 | ${machineStatus} | ${computationByCrc16Arc} | 0x79 |

    - MachineStatus: 0x00: Normal, 0x01: Error, 0x02: Locked, 0x03: Disconnected (Ping timeout)

4. **Pipeline Status (Protocol 0x33)**

    - Response:
      | START_BYTE | PROTOCOL_ID | DATA_LEN | PIPELINE_IDX | REMAIN_VOLUME[3:0] | CHECK_SUM | STOP_BYTE |
      | ---------- | ----------- | -------- | -------- | ----------- | -------- | ----------- |
      | 0x78 | 0x33 | 5 | ${pipelineIdx} |${remainVolume} | ${computationByCrc16Arc} | 0x79 |

    - PipelineIdx: Index of the pipeline
    - RemainVolume (float): Remain Volume after set Volume and start pushing

## Result Code

| RESULT_CODE | Description                                  |
| ----------- | -------------------------------------------- |
| 0x00        | RESULT_SUCCESS                               |
| 0x01        | RESULT_ERROR                                 |
| 0x10        | RESULT_COMM_PROTOCOL_ID_INVALID              |
| 0x11        | RESULT_COMM_PROTOCOL_CRC_INVALID             |
| 0x12        | RESULT_COMM_PROTOCOL_START_STOP_BYTE_INVALID |
| 0x13        | RESULT_COMM_PROTOCOL_DATA_LEN_INVALID        |
| 0x14        | RESULT_COMM_PROTOCOL_TIMEOUT                 |
| 0x20        | RESULT_CMD_RESET_TIMEOUT                     |
| 0x21        | RESULT_CMD_TRANSFER_OTA_DATA_IN_PROGRESS     |
| 0x30        | RESULT_CMD_SETTING_INVALID                   |
| 0x31        | RESULT_CMD_DEVICE_LOCKED                     |
| 0x32        | RESULT_CMD_PING_TIMEOUT                      |
| 0x33        | RESULT_CMD_KEY_INVALID                       |
