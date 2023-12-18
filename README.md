# n00b-scanner

This repository contains a simple port scanner written in Go (Golang). It's designed to scan a range of TCP ports on a specified IP address to check for open ports.

## Prerequisites

To run this port scanner, you need to have Go installed on your system. You can download and install Go from [the official Go website](https://golang.org/dl/).

## Installation

Clone the repository to your local machine:

```bash
git clone https://github.com/araujo88/n00b-scanner.git
cd n00b-scanner
```

## Usage

The port scanner can be executed directly through the Go command:

```bash
go run .
```

By default, the scanner is set to scan ports 80 to 100 on the IP `127.0.0.1` (localhost). You can modify the `main.go` file to scan different ports or a different IP address.

## Features

- Simple and fast port scanning.
- Concurrent scanning with Go's goroutines.
- Scans a range of ports on a specified IP address.

## Contributing

Contributions to this project are welcome! Feel free to fork the repository and submit pull requests.

## License

This project is licensed under the GPL License - see the [LICENSE](LICENSE) file for details.

## Disclaimer

Port scanning can be perceived as a hostile activity by network administrators. This tool is intended for educational purposes only. Always have explicit permission before scanning networks that you do not own or operate.

## TODOs

Check [TODOs.md](TODOs.md).
