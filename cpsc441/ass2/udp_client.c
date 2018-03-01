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

// Concatenates all information required for UDP block
char* create_udp_block(char* name, char* size, char* block, int leg, char* data, int datasize){
  // UDP block = Filename | File size | Block size |  Leg flag |  Data   
  char *UDP_block = (char*)malloc(datasize);
  char *separator = "|";

  strcat(UDP_block, name);
  strcat(UDP_block, separator);
  strcat(UDP_block, size);
  strcat(UDP_block, separator);
  strcat(UDP_block, block);
  strcat(UDP_block, separator);
  strcat(UDP_block, itoa(leg));
  strcat(UDP_block, separator);
  strcat(UDP_block, data);
  
  return UDP_block;
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
  char tiny_octo[8][1];
  char c;
  int leg, size = 0;
  int portions, remainder, has_tiny_blocks = 0;
  
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
    portions = size / 8;
    remainder = size % 8;

    if (remainder > 0)
      has_tiny_blocks = 1;
    
    int index = 0;

    for (int i = 0; i < 8; i++){
      for (int j = 0; j < portions; j++){
	octo[i][j] = buffer[index];
	index++;
      }
    }

    for (int i = 0; i < 8; i++){
      tiny_octo[i][1] = buffer[index];
      index++;
    }
  }

  printf("\n%s\n", octo[0]);
  
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

  // Create and send UDP blocks
  for (int i = 0; i < 1; i++){
    char *UDP_block = create_udp_block(argv[1], itoa(size), itoa(portions), i, octo[i], 1143);

    printf("%s", UDP_block);

    sendto(sock, UDP_block, sizeof(UDP_block),0,
  	   (struct sockaddr*)&server_address, sizeof(server_address));
  }

  // close the socket
  close(sock);
  return 0;
}

