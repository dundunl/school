/* udp_client.c
 * This is a modified version of udp_client.c provided in tutorial
 *
 * CPSC 441 Assignment 2: Octoput
 * Last modified: Feb. 28, 2018
 * Modified by: Duncan Lam
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define _256KB 262144
#define PORT 8001

//Converts integer to string
char* itoa(int i){
  int len = snprintf(NULL, 0, "%d", i);
  char *str = (char*)malloc(len + 1);
  sprintf(str, "%d", i);
  return str;
}

int main(int argc, char* argv[]) {
  // Check arguments for filename
  if (argc != 2){
    printf("Error: No file was provided.\nUsage: ./client <filename.txt>\n");
    exit(-1);
  }
  // Create octoblock buffers
  char buffer[_256KB];
  char octo[8][1111];
  char tiny_octo[8];
  char c;
  int leg, size = 0;
  
  // Open file and read into buffer
  FILE *fp;
  fp = fopen(argv[1], "r");
  if (fp == NULL){
    printf("Error: file does not exist.\n");
    exit(-1);
  }

  while (fscanf(fp, "%c", &c) != EOF){
    buffer[size] = c;
    size++;
  }

  fclose(fp);

  // Fill octolegs evenly if less than 8888 bytes
  if (size <= 8888){
    int portions = size / 8;
    int remainder = size % 8;
    int index = 0;

    for (int i = 0; i < 8; i++){
      for (int j = 0; j < portions; j++){
	octo[i][j] = buffer[index];
	index++;
      }
    }

    for (int i = 0; i < 8; i++){
      tiny_octo[i] = buffer[index];
      index++;
    }
  }
  
  const char* server_name = "localhost";//loopback
  const int server_port = PORT;
  
  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  
  // creates binary representation of server name
  // and stores it as sin_addr
  //inet_pton: convert IPv4 and IPv6 addresses from text to binary form
  
  inet_pton(AF_INET, server_name, &server_address.sin_addr);
  
  
  server_address.sin_port = htons(server_port);
  
  // open socket
  int sock;
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("could not create socket\n");
    return 1;
  }
  printf("client socket created\n");

  // send data
  // First, send the size of the file
  
  int len =
    sendto(sock, itoa(size), size, 0,
  	   (struct sockaddr*)&server_address, sizeof(server_address));
  printf("size of message has been sent to server\n");

  for (int i = 0; i < 8; i++){
    
  }
  
  // received echoed data back
  char recv_buffer[100];
  int recv_bytes=recvfrom(sock, recv_buffer, len, 0, NULL, NULL);
  printf("received bytes = %d\n",recv_bytes);
  recv_buffer[len] = '\0';
  printf("recieved: '%s'\n", recv_buffer);

  // close the socket
  close(sock);
  return 0;
}

