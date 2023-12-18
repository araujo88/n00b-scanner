# TODOs

1. **Command-Line Arguments**: Allow users to specify the target IP address and port range via command-line arguments instead of hardcoding them. This makes the tool more flexible and user-friendly.

2. **Concurrent Scans Limit**: Implement a feature to limit the number of concurrent scans. This can help prevent overwhelming the network and the target host.

3. **Verbose Mode**: Add a verbose mode that provides more detailed output for each scan, such as the type of error encountered when a port is closed or filtered.

4. **Scan Types**: Implement different types of scans, like UDP scanning, SYN scanning, etc., to make the tool more comprehensive.

5. **Service Detection**: Attempt to determine the service running on each open port (e.g., HTTP, FTP) and display this information.

6. **IP Range Scanning**: Allow scanning of a range of IP addresses, not just a single IP. This is useful for scanning multiple hosts on a network.

7. **Save Results to File**: Provide an option to save the scan results to a file in various formats like text, CSV, or JSON.

8. **Timeout Configuration**: Allow the user to configure the timeout for each port scan, providing flexibility for different network environments.

9. **Parallelism Control**: Implement a feature to control the level of parallelism (number of goroutines) based on user input or system capabilities.

10. **Port Description Database**: Include a small database or lookup feature that provides common uses for each port (e.g., Port 80 is commonly used for HTTP).

11. **Banner Grabbing**: Implement banner grabbing for open ports to gather more information about the services running on the target host.

12. **Scan Scheduling**: Allow users to schedule scans to run at specified times, useful for regular network monitoring.

13. **Interactive Mode**: Develop an interactive CLI mode where users can input commands and change settings on the fly during scanning.

14. **Integration with Network Tools**: Integrate with other network tools or libraries for enhanced scanning capabilities or data analysis.

15. **Graphical User Interface (GUI)**: Create a simple web-based or desktop GUI for users who prefer graphical interfaces over command-line tools.

16. **Network Map Visualization**: Provide a feature to visualize the network scan results, showing which ports are open on which hosts.

17. **Security Features**: Add security features like rate limiting and scan randomization to avoid detection by intrusion detection systems.

18. **Logging and Reporting**: Implement detailed logging and generate reports that summarize the scan results.

19. **Configurable Scan Profiles**: Allow users to create and save scan profiles with predefined settings for different scanning scenarios.

20. **Distributed Scanning**: Develop the capability for distributed scanning, where scans can be run from multiple network locations and aggregated.
