# CModem-Terminal

A powerful, lightweight C terminal for controlling GSM/GPRS modems through serial interfaces.  
Send SMS, manage calls, and execute AT commands reliably.

## Features

-   **Direct Serial Communication**: Reliable serial port connection to GSM/GPRS modems
-   **Interactive AT Terminal**: User-friendly interface for executing Hayes-compatible AT commands
-   **Robust Error Recovery**: Automatic retry with timeouts for failed modem initialization
-   **Comprehensive Logging**: Detailed logging of all command responses and modem events
-   **Call Management**: Real-time detection and logging of incoming/outgoing voice calls
-   **SMS Capabilities**: Full text-mode SMS sending and incoming message detection
-   **External Configuration**: Customizable AT command sequences via config file

## Requirements

-   **Compiler**: GCC with C11 support
-   **Dependencies**: POSIX threads (`-lpthread`)

## Building

To manually build and test the program locally, do the following:

```bash
git clone https://github.com/Valeriy-Kuvshinov/cmodem-terminal.git
cd cmodem-terminal
make
```

## Usage

```bash
# Connect to USB modem
./modem_terminal /dev/ttyUSB2

# Connect to serial modem with quiet mode
./modem_terminal /dev/ttyUSB2 -quiet
```

### Command Line Options

- `<serial_device>`: Serial port path (required, e.g., `/dev/ttyUSB0`)
- `--quiet`: Quiet mode - suppresses startup and instruction messages (optional)

### Interactive Commands

Once connected, you may send AT commands directly:

```bash
AT # Test modem connection
AT+CSQ # Check signal quality
ATD+1234567890; # Make a call
AT+CHUP # Hang up call
AT+CMGF=1 # Set SMS to text mode
AT+CMGS="+1234567890" # Send SMS
exit # Quit the terminal
```

## Configuration

The application uses `app_config.txt` for configuration.

### Supported Configuration Options

-   **baudrate**: Serial communication speed (default: 230400)
-   **command_N**: AT command to execute during initialization
-   **description_N**: Human-readable description of the AT command

## License

Copyright (C) Valeriy Kuvshinov.  
For private and commercial use.  
View [MIT LICENSE](LICENSE) for details.
