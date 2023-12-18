# n00b-scanner
A simple port scanner written in C++ using UNIX websockets API

## Build

`make clean` <br>
`make`

## Usage

`./n00bscanner <target_ip_address> <number_of_attempts> `

## Example

`./n00bscanner 127.0.0.1 1`

### Output

```
22      open
25      open
587     open
631     open
1716    open
5432    open
5433    open
5434    open
9050    open
46108   open
48580   open
```
