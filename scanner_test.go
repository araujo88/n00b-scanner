package main

import (
	"errors"
	"net"
	"strconv"
	"sync"
	"testing"
)

// MockDialer for testing
type MockDialer struct {
	DialFunc func(network, address string) (net.Conn, error)
}

func (m MockDialer) Dial(network, address string) (net.Conn, error) {
	return m.DialFunc(network, address)
}

func TestScanPort(t *testing.T) {
	var wg sync.WaitGroup
	testIP := "127.0.0.1"
	testPort := 8080

	mockDial := MockDialer{
		DialFunc: func(network, address string) (net.Conn, error) {
			expectedAddress := testIP + ":" + strconv.Itoa(testPort)
			if address != expectedAddress {
				t.Errorf("Dialing incorrect address, got: %s, want: %s", address, expectedAddress)
			}
			return nil, errors.New("port closed") // Simulate a closed port
		},
	}

	target := Target{
		ip:     testIP,
		port:   testPort,
		wg:     &wg,
		dialer: mockDial,
	}

	wg.Add(1)
	target.scanPort()
	wg.Wait()
}
