#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/tcp.h> // tcp headers
#include <netinet/ip.h>  // ip headers
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h> // select()

// #define MAX_PORT 65536

struct timeval tv;

volatile int scanning_done = 0;

typedef enum
{
  PORT_STATUS_NOT_SCANNED,
  PORT_STATUS_SCANNED,
  PORT_STATUS_RESPONSE_RECEIVED,
  PORT_STATUS_TIMEOUT
} port_status_t;

port_status_t *port_statuses;

const char *port_status_strings[] = {
    "Not Scanned",
    "Scanned",
    "Response Received",
    "Timeout"};

struct pseudo_header
{
  u_int32_t source_address;
  u_int32_t destination_address;
  u_int8_t placeholder;
  u_int8_t protocol;
  u_int16_t tcp_length;
};

int i;
int max_threads;
pthread_t *th;
pthread_mutex_t mutex;
int raw_socket;
int port_number;
char *pseudogram;
char datagram[4096];
char source_ip[32];
char *data;
struct iphdr *iph;
struct tcphdr *tcph;
struct sockaddr_in s_in;
struct pseudo_header psh;
int psize;
int one = 1;
const int *val = &one;

typedef struct
{
  int start_port;
  int end_port;
} port_range_t;

pthread_mutex_t port_statuses_mutex = PTHREAD_MUTEX_INITIALIZER;

void check_socket(int socket);
unsigned short csum(const char *buf, unsigned size);
void handle_signal(int sig);
void remove_char(char *string, char garbage);
int random_int(int min, int max);
void random_ip(char *string);
void scan(char **argv);
void *response_listener(void *arg);

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Please provide a destination IP address\n");
    exit(EXIT_FAILURE);
  }
  else if (argc < 3)
  {
    printf("Please provide a payload\n");
    exit(EXIT_FAILURE);
  }
  else if (argc < 4)
  {
    printf("Please provide the start port\n");
    exit(EXIT_FAILURE);
  }
  else if (argc < 5)
  {
    printf("Please provide the max port");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  printf("CONFIGURATION:\n");
  printf("IP address: %s\n", argv[1]);
  printf("Payload: %s\n", argv[2]);
  printf("Start port: %s\n", argv[3]);
  printf("Max port: %s\n\n", argv[4]);

  int num_ports = atoi(argv[4]) - atoi(argv[3]) + 1;
  port_statuses = malloc(num_ports * sizeof(port_status_t));
  for (int i = 0; i < num_ports; i++)
  {
    port_statuses[i] = PORT_STATUS_NOT_SCANNED;
  }

  port_range_t *port_range = malloc(sizeof(port_range_t));
  port_range->start_port = atoi(argv[3]);
  port_range->end_port = atoi(argv[4]);

  pthread_t listener_thread;
  if (pthread_create(&listener_thread, NULL, response_listener, port_range) != 0)
  {
    perror("Thread creation failed");
    free(port_range);
    return EXIT_FAILURE;
  }

  scan(argv);

  // Scanning is done, set the flag
  scanning_done = 1;

  pthread_join(listener_thread, NULL);

  for (int i = 0; i < num_ports; i++)
  {
    printf("Port %d - %s\n", i + port_range->start_port, port_status_strings[port_statuses[i]]);
  }

  free(port_range);
  free(port_statuses);
  pthread_mutex_destroy(&port_statuses_mutex);

  return 0;
}

void scan(char **argv)
{
  int start_port = atoi(argv[3]);
  int end_port = atoi(argv[4]);

  signal(SIGINT, handle_signal);
  for (port_number = start_port; port_number <= end_port; port_number++)
  {
    // raw socket
    raw_socket = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    check_socket(raw_socket);

    memset(datagram, 0, sizeof(datagram));

    iph = (struct iphdr *)datagram;

    tcph = (struct tcphdr *)(datagram + sizeof(struct iphdr));

    data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
    char *target_ip = ((char *)data);

    strcpy(data, argv[2]);
    // address resolution
    random_ip(source_ip); // spoofed IP address - ex: 192.168.1.2
    s_in.sin_family = AF_INET;
    s_in.sin_port = htons(port_number);          // port
    s_in.sin_addr.s_addr = inet_addr(target_ip); // target IP

    // IP header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(data);
    iph->id = htonl(54321); // packet id
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;
    iph->saddr = inet_addr(source_ip); // source ip address
    iph->daddr = s_in.sin_addr.s_addr; // destination ip address

    // IP checksum
    iph->check = csum(datagram, iph->tot_len);

    // TCP header
    remove_char(source_ip, '.');
    tcph->source = htons(atoi(source_ip));
    tcph->dest = htons(port_number);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5; // tcp header size
    tcph->fin = 0;
    tcph->syn = 1;
    tcph->rst = 0;
    tcph->psh = 0;
    tcph->ack = 0;
    tcph->urg = 0;
    tcph->window = htons(5840); // maximum allowed window size
    tcph->check = 0;            // leave checksum 0 now, filled later by pseudo header
    tcph->urg_ptr = 0;

    // TCP checksum
    psh.source_address = inet_addr(source_ip);
    psh.destination_address = s_in.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr) + strlen(data));

    psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
    pseudogram = malloc(psize);

    memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), tcph, sizeof(struct tcphdr) + strlen(data));

    tcph->check = csum(pseudogram, psize);

    // IP_HDRINCL to tell the kernel that headers are included in the packet
    if (setsockopt(raw_socket, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
    {
      perror("Error setting IP_HDRINCL");
      printf("Error code: %d\n", errno);
      exit(EXIT_FAILURE);
    }

    // Send the packet
    if (sendto(raw_socket, datagram, iph->tot_len, 0,
               (struct sockaddr *)&s_in, sizeof(s_in)) < 0)
    {
      perror("sendto failed");
    }
    else
    {
      printf("SYN Packet Sent to port %d\n", port_number);
    }
    // sleep for 3 seconds
    sleep(5);
    close(raw_socket);
    pthread_mutex_lock(&port_statuses_mutex);
    port_statuses[port_number - start_port] = PORT_STATUS_SCANNED;
    pthread_mutex_unlock(&port_statuses_mutex);
  }
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

unsigned short csum(const char *buf, unsigned size)
{
  unsigned long long sum = 0;
  const unsigned long long *b = (unsigned long long *)buf;

  unsigned t1, t2;
  unsigned short t3, t4;

  /* Main loop - 8 bytes at a time */
  while (size >= sizeof(unsigned long long))
  {
    unsigned long long s = *b++;
    sum += s;
    if (sum < s)
      sum++;
    size -= 8;
  }

  /* Handle tail less than 8-bytes long */
  buf = (const char *)b;
  if (size & 4)
  {
    unsigned s = *(unsigned *)buf;
    sum += s;
    if (sum < s)
      sum++;
    buf += 4;
  }

  if (size & 2)
  {
    unsigned short s = *(unsigned short *)buf;
    sum += s;
    if (sum < s)
      sum++;
    buf += 2;
  }

  if (size)
  {
    unsigned char s = *(unsigned char *)buf;
    sum += s;
    if (sum < s)
      sum++;
  }

  /* Fold down to 16 bits */
  t1 = sum;
  t2 = sum >> 32;
  t1 += t2;
  if (t1 < t2)
    t1++;
  t3 = t1;
  t4 = t1 >> 16;
  t3 += t4;
  if (t3 < t4)
    t3++;

  return ~t3;
}

void handle_signal(int sig)
{
  printf("\nCaught interrupt signal %d\n", sig);
  puts("Releasing resources ...");
  free(pseudogram);
  free(th);
  for (i = 0; i < max_threads; i++)
  {
    if (pthread_join(th[i], NULL) != 0)
    {
      exit(EXIT_FAILURE);
    }
  }
  // closes the socket
  puts("Closing socket ...");
  if (close(raw_socket) == 0)
  {
    puts("Socket closed!");
    exit(EXIT_SUCCESS);
  }
  else
  {
    perror("An error occurred while closing the socket: ");
    printf("Error code: %d\n", errno);
    exit(EXIT_FAILURE);
  }
}

void remove_char(char *string, char garbage)
{

  char *src, *dst;
  for (src = dst = string; *src != '\0'; src++)
  {
    *dst = *src;
    if (*dst != garbage)
      dst++;
  }
  *dst = '\0';
}

int random_int(int min, int max)
{
  int k;
  double d;
  d = (double)rand() / ((double)RAND_MAX + 1);
  k = d * (max - min + 1);
  return min + k;
}

void random_ip(char *string)
{
  int octet1 = random_int(0, 255);
  int octet2 = random_int(0, 255);
  int octet3 = random_int(0, 255);
  int octet4 = random_int(0, 255);

  sprintf(string, "%d.%d.%d.%d", octet1, octet2, octet3, octet4);
}

void *response_listener(void *arg)
{
  port_range_t *port_range = (port_range_t *)arg;
  int start_port = port_range->start_port;
  int end_port = port_range->end_port;
  int sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
  if (sock_raw < 0)
  {
    perror("Socket Error");
    return NULL;
  }

  unsigned char *buffer = (unsigned char *)malloc(65536);
  struct sockaddr saddr;
  int saddr_size;

  while (!scanning_done)
  {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock_raw, &readfds);

    struct timeval tv;
    tv.tv_sec = 5; // Set timeout to 3 seconds
    tv.tv_usec = 0;

    int rv = select(sock_raw + 1, &readfds, NULL, NULL, &tv);
    if (rv == -1)
    {
      perror("Select error");
      break; // Exit on select error
    }
    else if (rv == 0)
    {
      for (int i = 0; i < port_range->end_port - port_range->start_port; i++)
      {
        if (port_statuses[i] == PORT_STATUS_SCANNED)
        {
          pthread_mutex_lock(&port_statuses_mutex);
          port_statuses[i] = PORT_STATUS_TIMEOUT;
          pthread_mutex_unlock(&port_statuses_mutex);
        }
      }
    }

    saddr_size = sizeof(saddr);
    int data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_size);
    if (data_size < 0)
    {
      perror("Recvfrom error");
      continue;
    }

    struct iphdr *iph = (struct iphdr *)buffer;
    if (iph->protocol == IPPROTO_TCP)
    {
      if (iph->ihl < 5)
        continue; // Not a valid IP header

      struct tcphdr *tcph = (struct tcphdr *)(buffer + iph->ihl * 4);
      unsigned short dest_port = ntohs(tcph->dest);
      int port_index = dest_port - start_port;
      int num_ports = end_port - start_port;

      if (port_index >= 0 && port_index < num_ports)
      {
        pthread_mutex_lock(&port_statuses_mutex);
        port_statuses[port_index] = PORT_STATUS_RESPONSE_RECEIVED;
        pthread_mutex_unlock(&port_statuses_mutex);
      }

      if (dest_port >= start_port && dest_port <= end_port)
      {
        if (tcph->syn == 1 && tcph->ack == 1)
        {
          printf("Port %d is open (SYN-ACK received)\n", dest_port);
        }
        else if (tcph->rst == 1)
        {
          printf("Port %d is closed (RST received)\n", dest_port);
        }
      }
    }
  }

  close(sock_raw);
  free(buffer);
  return NULL;
}
