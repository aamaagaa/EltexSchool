#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define CLIENT_PORT 7777
#define SERVER_PORT 8888
#define SHUTDOWN_MSG "shutdown"

int fd;
struct sockaddr_in server_addr;

void cleanup() {
    char buffer[sizeof(struct iphdr) + sizeof(struct udphdr) + BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    struct iphdr *ip_header = (struct iphdr *)buffer;
    ip_header->ihl = 5;
    ip_header->version = 4;
    ip_header->tos = 0;
    ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(SHUTDOWN_MSG));
    ip_header->id = htons(12345);
    ip_header->frag_off = 0;
    ip_header->ttl = 64;
    ip_header->protocol = IPPROTO_UDP;
    
    struct in_addr src_addr, dst_addr;
    inet_pton(AF_INET, "127.0.0.1", &src_addr);
    inet_pton(AF_INET, "127.0.0.1", &dst_addr);
    
    ip_header->saddr = src_addr.s_addr;
    ip_header->daddr = dst_addr.s_addr;
    ip_header->check = 0;

    struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct iphdr));
    udp->source = htons(CLIENT_PORT);
    udp->dest = htons(SERVER_PORT);
    udp->len = htons(sizeof(struct udphdr) + strlen(SHUTDOWN_MSG));
    udp->check = 0;

    char *data = buffer + sizeof(struct iphdr) + sizeof(struct udphdr);
    memcpy(data, SHUTDOWN_MSG, strlen(SHUTDOWN_MSG));

    sendto(fd, buffer, ntohs(ip_header->tot_len), 0, 
           (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    close(fd);
}

void signalHandler(int signum) {
    cleanup();
    exit(signum);
}

void send_message(char *message) {
    char buffer[sizeof(struct iphdr) + sizeof(struct udphdr) + BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    int data_len = strlen(message);

    struct iphdr *ip_header = (struct iphdr *)buffer;
    ip_header->ihl = 5;
    ip_header->version = 4;
    ip_header->tos = 0;
    ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + data_len);
    ip_header->id = htons(12345);
    ip_header->frag_off = 0;
    ip_header->ttl = 64;
    ip_header->protocol = IPPROTO_UDP;
    
    struct in_addr src_addr, dst_addr;
    inet_pton(AF_INET, "127.0.0.1", &src_addr);
    inet_pton(AF_INET, "127.0.0.1", &dst_addr);
    
    ip_header->saddr = src_addr.s_addr;
    ip_header->daddr = dst_addr.s_addr;
    ip_header->check = 0;

    struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct iphdr));
    udp->source = htons(CLIENT_PORT);
    udp->dest = htons(SERVER_PORT);
    udp->len = htons(sizeof(struct udphdr) + data_len);
    udp->check = 0;

    char *data = buffer + sizeof(struct iphdr) + sizeof(struct udphdr);
    memcpy(data, message, data_len);

    if (sendto(fd, buffer, ntohs(ip_header->tot_len), 0, 
              (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("sendto");
        return;
    }

    char recv_buffer[BUFFER_SIZE];
    int timeout = 5;
    fd_set readfds;
    struct timeval tv;
    
    while(timeout > 0) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        int ret = select(fd + 1, &readfds, NULL, NULL, &tv);
        
        if (ret == -1) {
            perror("select");
            break;
        } else if (ret == 0) {
            timeout--;
            continue;
        }
        
        if (FD_ISSET(fd, &readfds)) {
            struct sockaddr_in from_addr;
            socklen_t from_len = sizeof(from_addr);
            
            int bytes_received = recvfrom(fd, recv_buffer, sizeof(recv_buffer), 0, 
                                         (struct sockaddr *)&from_addr, &from_len);
            
            if (bytes_received == -1) {
                perror("recvfrom");
                continue;
            }

            struct iphdr *recv_ip = (struct iphdr *)recv_buffer;
            int ip_header_len = recv_ip->ihl * 4;
            
            if (recv_ip->protocol == IPPROTO_UDP) {
                struct udphdr *recv_udp = (struct udphdr *)(recv_buffer + ip_header_len);
                
                if (ntohs(recv_udp->dest) == CLIENT_PORT) {
                    char *recv_data = recv_buffer + ip_header_len + sizeof(struct udphdr);
                    int recv_data_len = bytes_received - ip_header_len - sizeof(struct udphdr);
                    recv_data[recv_data_len] = '\0';
                    
                    printf("message from server: %s\n", recv_data);
                    return;
                }
            }
        }
    }
    
}

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: %s <server_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip = argv[1];

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

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    signal(SIGINT, signalHandler);
    atexit(cleanup);

    char input[BUFFER_SIZE];
    while(1) {
        printf("\nenter message: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) > 0) {
            send_message(input);
        }
    }

    return 0;
}