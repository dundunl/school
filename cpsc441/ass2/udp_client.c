/* udp_client.c
 * This is a modified version of udp_client.c provided in tutorial
 *
 * CPSC 441 Assignment 2: Octoput
 * Last modified: Feb. 28, 2018
 * Modified by: Duncan Lam
 */

#include <arpa/inet.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>

#define _256KB 262144
#define PORT 8001

// Converts integer to string
char* itoa(int i){
  int len = snprintf(NULL, 0, "%d", i);
  char *str = (char*)malloc(len + 1);
  sprintf(str, "%d", i);
  return str;
}

// Converts integer to character
char itoc(int i){
  return (char)(1 << i);
}

// Concatenates all information required for UDP block
char* create_udp_block(char* name, char* size, char* block, char leg, char* data, int datasize){
  // UDP block = Filename | File size | Block size |  Leg flag |  Data   
  char *UDP_block = (char*)malloc(datasize);
  char *separator = "|";
  memset(UDP_block, '\0', datasize);

  strcat(UDP_block, name);
  strcat(UDP_block, separator);
  strcat(UDP_block, size);
  strcat(UDP_block, separator);
  strcat(UDP_block, block);
  strcat(UDP_block, separator);
  strcat(UDP_block, &leg);
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
  // Create octoblock buffers - allow enough room for string + '\0'
  char buffer[_256KB+1];
  char octo[8][1112];
  char tiny_octo[8][2];
  char c;
  int leg, size = 0;
  int portions, remainder, has_tiny_blocks = 0;
  int blocks = 1;
  
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

  const char* server_name = "localhost";//loopback
  const int server_port = PORT;
  
  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  int server_address_len = sizeof(server_address_len);
  
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

  for (int k = 0; k < blocks; k++){
    // Fill octolegs evenly if less than 8888 bytes
    if (size <= 8888){
      portions = size / 8;
      remainder = size % 8;

      if (remainder > 0){
	has_tiny_blocks = 1;
	blocks = 2;
      }

      for (int i = 0; i < 8; i++)
	strncpy(octo[i], &buffer[i*portions], portions);

      if (has_tiny_blocks)
	for (int i = 0; i < 8; i++)
	  strncpy(tiny_octo[i], &buffer[portions*8 + i], 1);
    }
    // Fill full octoblocks then partial and tiny octoblocks
    // if more than 8888 bytes
    else{
      blocks = (int)ceil((double)size / 8888.0);
      int blockchunk = size - 8888 * k;
      portions = (blockchunk < 8888) ? blockchunk/8 : 1111;
      remainder = (portions == 1111) ? 0 : blockchunk % 8;
      printf("Blocks: %d, leg size: %d, remainder: %d\n", blocks, portions, remainder);

      if (remainder > 0){
	has_tiny_blocks = 1;
	blocks++;
      }

      for (int i = 0; i < 8; i++)
	strncpy(octo[i], &buffer[i*portions + 8888*k], portions);

      if (has_tiny_blocks)
	for (int i = 0; i < 8; i++){
	  strncpy(tiny_octo[i], &buffer[size - remainder + i], 1);
	}
    }

    /* Create multiprocess sending:
     * Parent process transmits and retransmits data if necessary
     * child process p_ack handles acknowledgements of received data
     * child process p_tout handles timeouts 
    */
    pid_t p_ack;
    int ack[2];

    if(pipe(ack) == -1){
      printf("Error creating pipe.\n");
      exit(-1);
    }

      // Create and send UDP blocks
      for (int i = 0; i < 8; i++){
	if((p_ack = fork()) == -1){
	  printf("Error creating ACK Process.\n");
	  exit(-1);
	}
	
	//Acknowledge process
	if(p_ack == 0){
	  char ack_leg = '9';
	  int recv = 0;
	  struct timespec stop, start;
	  long int timer_ms = 0;
      
	  clock_gettime(CLOCK_MONOTONIC, &start);
      
	  //Wait for acknowledgement from server or timeout of 1s
	  while(1){
	    recv = recvfrom(sock, &ack_leg, sizeof(char), MSG_DONTWAIT,
			    (struct sockaddr*)&server_address, &server_address_len);

	    clock_gettime(CLOCK_MONOTONIC, &stop);
	    timer_ms = (stop.tv_sec - start.tv_sec) * 1000000
	      + (stop.tv_nsec - start.tv_nsec) / 1000;
	    
	    if(recv > 0 || timer_ms > 1000){
	      break;
	    }
	  }

	  //Send acknowledgement to parent
	  write(ack[1], &ack_leg, sizeof(ack_leg));
	  exit(0);
	}
	// Parent process
	else{
	  char ack_leg;
	  
	  //Send tiny blocks if they are the last remaining blocks to send
	  if (k == blocks - 1 && has_tiny_blocks){
	    char *UDP_block = create_udp_block(argv[1], itoa(size),
					       itoa(remainder), itoc(i), tiny_octo[i], 100);
	    sendto(sock, UDP_block, strlen(UDP_block),0,
		   (struct sockaddr*)&server_address, sizeof(server_address));
	  }
	  // else, Send full blocks
	  else{
	    char *UDP_block = create_udp_block(argv[1], itoa(size),
					       itoa(portions), itoc(i), octo[i], 2000);
	    int len = sendto(sock, UDP_block, strlen(UDP_block),0,
			     (struct sockaddr*)&server_address, sizeof(server_address));
	  }

	  wait(NULL);
	  read(ack[0], &ack_leg, sizeof(ack_leg));

	  // Resend if transmission timed out
	  if(atoi(&ack_leg) == 9){
	    i--;
	  }
	}
      }
  
      char* done = "done";
      //send message to indicate UDP send completion
      sendto(sock, done, strlen(done),0,
	     (struct sockaddr*)&server_address, sizeof(server_address));

  }
  printf("File \"%s\" successfully transferred.\n", argv[1]); 

  // close the socket
  close(sock);
  return 0;
}

