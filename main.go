package main

import (
	"fmt"
	"net"
	"strconv"
	"sync"
	"time"
)

func scanPort(ip string, port int, wg *sync.WaitGroup) {
	defer wg.Done()
	address := ip + ":" + strconv.Itoa(port)
	conn, err := net.DialTimeout("tcp", address, 1*time.Second)

	if err != nil {
		fmt.Printf("Port %d: Closed\n", port)
		return
	}

	defer conn.Close()
	fmt.Printf("Port %d: Open\n", port)
}

func main() {
	var wg sync.WaitGroup

	// Define the target IP and port range
	targetIP := "127.0.0.1" // Replace with your target IP
	startPort := 80         // Replace with your start port
	endPort := 100          // Replace with your end port

	for port := startPort; port <= endPort; port++ {
		wg.Add(1)
		go scanPort(targetIP, port, &wg)
	}

	// Wait for a bit to allow all goroutines to finish
	wg.Wait()
}
