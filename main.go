package main

import (
	"sync"
)

func main() {
	var wg sync.WaitGroup

	// Define the target IP and port range
	targetIP := "127.0.0.1" // Replace with your target IP
	startPort := 80         // Replace with your start port
	endPort := 100          // Replace with your end port

	for port := startPort; port <= endPort; port++ {
		target := Target{
			ip:     targetIP,
			port:   port,
			wg:     &wg,
			dialer: NetDialer{},
		}
		wg.Add(1)
		go target.scanPort()
	}

	// Wait for a bit to allow all goroutines to finish
	wg.Wait()
}
