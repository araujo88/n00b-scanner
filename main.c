#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_PORT 65536

int port_number = 0;

// timeout struct
struct timeval timeout;

void check_socket(int socket);

int main(int argc, char *argv[])
{
  int web_socket;
  int synRetries; // total number of SYN packets
  struct sockaddr_in s_in;

  if (argc < 2)
  {
    printf("Please provide a target IP address\n");
    exit(EXIT_FAILURE);
  }
  if (argc < 3)
  {
    printf("Please provide a maximum number of attempts\n");
    exit(EXIT_FAILURE);
  }

  synRetries = atoi(argv[2]);

  s_in.sin_family = AF_INET;
  s_in.sin_addr.s_addr = inet_addr(argv[1]); // target IP

  timeout.tv_sec = 5;  // 5 seconds
  timeout.tv_usec = 0; // 0 microseconds

  for (port_number = 1; port_number < MAX_PORT; port_number++)
  {
    web_socket = socket(AF_INET, SOCK_STREAM, 0);
    check_socket(web_socket);

    // Set the socket to non-blocking mode
    fcntl(web_socket, F_SETFL, O_NONBLOCK);

    s_in.sin_port = htons(port_number); // port

    setsockopt(web_socket, IPPROTO_TCP, TCP_SYNCNT, &synRetries, sizeof(synRetries));

    if (connect(web_socket, (struct sockaddr *)&s_in, sizeof(s_in)) < 0)
    {
      if (errno != EINPROGRESS)
      {
        // Handle immediate connection failure
        //  printf("%d\tconnection failed immediately\n", port_number);
        close(web_socket);
        continue;
      }
    }

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(web_socket, &fdset);

    fd_set errset;
    FD_ZERO(&errset);
    FD_SET(web_socket, &errset);

    if (select(web_socket + 1, NULL, &fdset, &errset, &timeout) > 0)
    {
      int so_error;
      socklen_t len = sizeof(so_error);

      getsockopt(web_socket, SOL_SOCKET, SO_ERROR, &so_error, &len);

      if (so_error == 0)
      {
        printf("%d\topen\n", port_number);
      }
      else
      {
        // printf("%d\tclosed/filtered\n", port_number);
      }
    }
    else
    {
      // printf("%d\ttimeout or error\n", port_number);
    }

    close(web_socket);
  }

  return 0;
}

void check_socket(int socket)
{
  if (socket < 0)
  {
    perror("Socket criation error");
    printf("Error code: %d\n", errno);
    exit(EXIT_FAILURE);
  }
}
