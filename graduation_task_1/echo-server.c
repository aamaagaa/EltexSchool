#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <time.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define SERVER_PORT 8888
#define MAX_CLIENTS 100
#define SHUTDOWN_MSG "shutdown"

int fd;

typedef struct {
    uint32_t ip;
    uint16_t port;
    int message_count;
} client_info_t;

client_info_t clients[MAX_CLIENTS];
int client_count = 0;

void signalHandler(int signum) {
    close(fd);
    exit(signum);
}

client_info_t* find_client(uint32_t ip, uint16_t port) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].ip == ip && clients[i].port == port) {
            return &clients[i];
        }
    }
    return NULL;
}

client_info_t* add_client(uint32_t ip, uint16_t port) {
    if (client_count >= MAX_CLIENTS) {
        return NULL;
    }
    
    clients[client_count].ip = ip;
    clients[client_count].port = port;
    clients[client_count].message_count = 0;
    
    return &clients[client_count++];
}

void remove_client(uint32_t ip, uint16_t port) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].ip == ip && clients[i].port == port) {
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j + 1];
            }
            client_count--;
            printf("%s:%d removed\n", inet_ntoa(*(struct in_addr*)&ip), ntohs(port));
            break;
        }
    }
}

int main() {
    char buffer[BUFFER_SIZE];
    
    memset(clients, 0, sizeof(clients));
    
    fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int one = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        close(fd);
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, signalHandler);
    printf("server listening on port %d\n", SERVER_PORT);

    while(1){
        memset(buffer, 0, BUFFER_SIZE);
        
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int bytes_received = recvfrom(fd, buffer, BUFFER_SIZE, 0, 
                                     (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received == -1) {
            perror("recvfrom");
            continue;
        }

        struct iphdr *ip_header = (struct iphdr *)buffer;
        int ip_header_len = ip_header->ihl * 4;
        
        if (ip_header->protocol != IPPROTO_UDP) {
            continue;
        }

        struct udphdr *udp_header = (struct udphdr *)(buffer + ip_header_len);
        
        if (ntohs(udp_header->dest) == SERVER_PORT) {
            char *data = buffer + ip_header_len + sizeof(struct udphdr);
            int data_len = bytes_received - ip_header_len - sizeof(struct udphdr);
            data[data_len] = '\0';
            
            uint32_t client_ip = ip_header->saddr;
            uint16_t client_port = udp_header->source;
            
            printf("%s:%d: %s\n", 
                   inet_ntoa(*(struct in_addr*)&client_ip),
                   ntohs(client_port),
                   data);

            if (strcmp(data, SHUTDOWN_MSG) == 0) {
                remove_client(client_ip, client_port);
                continue;
            }

            client_info_t *client = find_client(client_ip, client_port);
            if (client == NULL) {
                client = add_client(client_ip, client_port);
                printf("new client: %s:%d\n", 
                       inet_ntoa(*(struct in_addr*)&client_ip), ntohs(client_port));
            }
            
            if (client != NULL) {
                client->message_count++;
                
                char response[BUFFER_SIZE];
                snprintf(response, sizeof(response), "%s %d", data, client->message_count);
                int response_len = strlen(response);

                char send_buffer[BUFFER_SIZE];
                memset(send_buffer, 0, BUFFER_SIZE);
                
                struct iphdr *send_ip = (struct iphdr *)send_buffer;
                send_ip->ihl = 5;
                send_ip->version = 4;
                send_ip->tos = 0;
                send_ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + response_len);
                send_ip->id = htons(54321);
                send_ip->frag_off = 0;
                send_ip->ttl = 64;
                send_ip->protocol = IPPROTO_UDP;
                send_ip->saddr = ip_header->daddr;
                send_ip->daddr = ip_header->saddr;
                send_ip->check = 0;
                
                struct udphdr *send_udp = (struct udphdr *)(send_buffer + sizeof(struct iphdr));
                send_udp->source = htons(SERVER_PORT);
                send_udp->dest = client_port;
                send_udp->len = htons(sizeof(struct udphdr) + response_len);
                send_udp->check = 0;
                
                memcpy(send_buffer + sizeof(struct iphdr) + sizeof(struct udphdr), response, response_len);
                
                struct sockaddr_in dest_addr;
                memset(&dest_addr, 0, sizeof(dest_addr));
                dest_addr.sin_family = AF_INET;
                dest_addr.sin_addr.s_addr = send_ip->daddr;
                
                if (sendto(fd, send_buffer, ntohs(send_ip->tot_len), 0,
                          (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
                    perror("sendto");
                }
            }
        }
    }

    return 0;
}