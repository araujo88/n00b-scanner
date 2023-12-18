# n00b-scanner

A simple port scanner written in C++ using UNIX websockets API

## Overview

This repository contains a multi-threaded port scanner written in C. The scanner sends TCP SYN packets to a range of ports on a specified target IP address and listens for responses to determine if the ports are open, closed, or filtered. It is designed for educational and testing purposes to understand network programming and port scanning mechanics.

## Features

- Sends TCP SYN packets to a user-defined range of ports.
- Multi-threaded implementation with separate threads for scanning and response listening.
- Customizable target IP address and port range.
- Detection of open, closed, and filtered ports.

## Prerequisites

- GCC (GNU Compiler Collection) or any standard C compiler.
- A Unix-like environment (Linux, BSD, macOS) with standard libraries.
- Root privileges for raw socket operations.

## Installation

Clone the repository to your local machine:

```bash
git clone https://github.com/yourusername/multi-threaded-port-scanner.git
cd multi-threaded-port-scanner
```

## Usage

To compile the program:

```bash
make clean
make
```

To run the scanner (requires root privileges):

```bash
sudo ./port_scanner <Target-IP> <Payload> <Start-Port> <End-Port>
```

- `<Target-IP>`: IP address of the target machine.
- `<Payload>`: A string payload to send.
- `<Start-Port>`: Starting port number of the scan range.
- `<End-Port>`: Ending port number of the scan range.

Example:

```bash
sudo ./port_scanner 192.168.1.1 "Hello" 80 100
```

## Disclaimer

This tool is for educational and testing purposes only. Unauthorized scanning and penetration testing are illegal and unethical. Ensure you have explicit permission to scan the network and systems.

## Contributing

Contributions to this project are welcome. Please ensure you adhere to the coding standards and include proper documentation with your pull requests.

## License

This project is licensed under the [LICENSE](LICENSE).
