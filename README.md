# CModem-Terminal

A lightweight, thread-safe C terminal for controlling GSM/GPRS modems through serial interfaces.  
Send SMS, manage calls, and execute AT commands reliably.

## Features

- **Direct Serial Communication**: Reliable serial port connection to GSM/GPRS modems using non-blocking I/O.
- **Interactive AT Terminal**: User-friendly interface for executing Hayes-compatible AT commands.
- **Command History**: Navigate previously executed commands using the Up/Down arrow keys (History capped at 500 commands).
- **Robust Concurrency**: Thread-safe architecture utilizing C11 atomic operations and I/O multiplexing (`poll`).
- **Comprehensive Logging**: Detailed logging of all command responses and modem events, formatted with ISO 8601 timestamps and ANSI escape sequences.
- **Call Management**: Real-time detection and logging of incoming/outgoing voice calls.
- **SMS Capabilities**: Full text-mode SMS sending and incoming message detection.
- **External Configuration**: Optional parsing of startup AT command sequences and baud rate via config file.

## Requirements

- **Compiler**: GCC with strict C11 support
- **OS**: Linux (POSIX compliant)
- **Dependencies**:
    - POSIX threads (`-lpthread`)
    - GNU Readline (`-lreadline`)

## Building

GNU Readline development headers are needed in order to build the program.

```bash
# Install required dependencies
sudo apt-get install libreadline-dev

# Clone and build
git clone [https://github.com/Valeriy-Kuvshinov/cmodem-terminal.git](https://github.com/Valeriy-Kuvshinov/cmodem-terminal.git)
cd cmodem-terminal
make
```

## Usage

```bash
# Connect to USB modem (interactive mode)
./modem_terminal /dev/ttyUSB2

# Connect and execute initialization commands from config
./modem_terminal /dev/ttyUSB2 --config

# Connect in headless server mode (suppress timestamps, instructions, and ANSI formatting)
./modem_terminal /dev/ttyUSB2 --server

# Combine optional flags (recommended for servers)
./modem_terminal /dev/ttyUSB2 --config --server
```

### Command Line Options

- `<serial_device>`: Serial port path (required, e.g., /dev/ttyUSB0)
- `--config`: Load baud rate and initialization sequence from app_config.txt (optional, defaults to 230400 baud if omitted)
- `--server`: Headless server execution - suppresses startup messages, timestamps, and interactive ANSI formatting (optional)

### Interactive Commands

Once connected, you may send AT commands directly:

```bash
AT # Test modem connection
AT+CSQ # Check signal quality
ATD+1234567890; # Make a call
AT+CHUP # Hang up call
AT+CMGF=1 # Set SMS to text mode
AT+CMGS="+1234567890" # Send SMS
exit # Safely shutdown the threads and quit the terminal
```

## Configuration

The application uses `app_config.txt` for configuration.

### Supported Configuration Options

- **baudrate**: Serial communication speed (default: 230400)
- **command_N**: AT command to execute during initialization
- **description_N**: Human-readable description of the AT command

## License

Copyright (C) Valeriy Kuvshinov.  
View [MIT LICENSE](LICENSE) for details.
