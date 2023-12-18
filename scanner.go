package main

import (
	"fmt"
	"net"
	"strconv"
	"sync"
)

// Dialer interface abstracts the network dialing
type Dialer interface {
	Dial(network, address string) (net.Conn, error)
}

// NetDialer implements Dialer using actual network operations
type NetDialer struct{}

func (NetDialer) Dial(network, address string) (net.Conn, error) {
	return net.Dial(network, address)
}

// Target now includes a Dialer
type Target struct {
	ip     string
	port   int
	wg     *sync.WaitGroup
	dialer Dialer
}

func (target Target) scanPort() {
	defer target.wg.Done()
	address := target.ip + ":" + strconv.Itoa(target.port)
	conn, err := target.dialer.Dial("tcp", address)

	if err != nil {
		fmt.Printf("Port %d: Closed\n", target.port)
		return
	}

	conn.Close()
	fmt.Printf("Port %d: Open\n", target.port)
}
