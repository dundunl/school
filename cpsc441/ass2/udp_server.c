/* udp_server.c
 * This is a modified version of udp_server.c provided in tutorial
 *
 * CPSC 441 Assignment 2: Octoput
 * Last modified: Feb. 28, 2018
 * Modified by: Duncan Lam
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PORT 8001

// Converts integer to string
char* itoa(int i){
  int len = snprintf(NULL, 0, "%d", i);
  char *str = (char*)malloc(len + 1);
  sprintf(str, "%d", i);
  return str;
}

int main(int argc, char *argv[]) {
  // port to start the server on
  int SERVER_PORT = PORT;

  // socket address used for the server
  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;

  // htons: host to network short: transforms a value in host byte
  // ordering format to a short value in network byte ordering format
  server_address.sin_port = htons(SERVER_PORT);

  // htons: host to network long: same as htons but to long
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);

  // create a UDP socket, creation returns -1 on failure
  int sock;
  if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("could not create socket\n");
    return 1;
  }
  printf("server socket created\n");
  // bind it to listen to the incoming connections on the created server
  // address, will return -1 on error
  if ((bind(sock, (struct sockaddr *)&server_address,
	    sizeof(server_address))) < 0) {
    printf("could not bind socket\n");
    return 1;
  }
  printf("binding was successful\n");
  // socket address used to store client address
  struct sockaddr_in client_address;
  int client_address_len = sizeof(client_address);
  char client_name[100];
  char octolegs[8][1112];
  char octoblock[8888];
  int transfer_complete = 0;
  char filename[100];

  // run indefinitely
  while (true) {
     char buffer[2000];
     char *splitter;
     char *filesize, *legsize, leg_char,  *data;
     int leg_num;

     // Clear buffer
     memset(buffer, '\0', sizeof(buffer));

     // Receive UDP block transfer
     recvfrom(sock, buffer, sizeof(buffer), 0,
	      (struct sockaddr *)&client_address, &client_address_len);

     // If client is not done sending data, parse UDP block
     if (strcmp("done", buffer) != 0){
       splitter = strtok(buffer, "|");
       filesize = strtok(NULL, "|");
       legsize = strtok(NULL, "|");
       leg_char = *strtok(NULL, "|");
       data = strtok(NULL, "|");

       strcpy(filename, splitter);

       leg_num = (int)log2(abs((double)leg_char));

       // Acknowledge by sending back leg number
       sendto(sock, itoa(leg_num), sizeof(itoa(leg_num)), 0,
	      (struct sockaddr *)&client_address, sizeof(client_address));

       if (data != NULL){
	 strcpy(octolegs[leg_num], data);
	 octolegs[leg_num][atoi(legsize)] = '\0';
       }
     }

     // if client is done sending data, write data to file
     if (strcmp("done", buffer) == 0){
       // Form Octoblock
       memset(octoblock, '\0', sizeof(octoblock));
       for (int i = 0; i < 8; i++){
	 strcat(octoblock, octolegs[i]);
	 memset(octolegs[i],'\0', strlen(octolegs[i]));
       }

       // Create new folder for files (check if folder exists first)
       struct stat st = {0};
       if (stat("./downloads", &st) == -1){
	 mkdir("./downloads", 0700);
       }

       // Open new file in directory   
       char filepath[100] = "./downloads/";
       strcat(filepath, filename);

       // Write octoblock to file
       FILE *f_output = fopen(filepath, "rw+");

       // If file exists, seek to end
       if (f_output != NULL){
	 fseek(f_output, 0, SEEK_END);
	 //printf("Writing to position: %ld\n", ftell(f_output));
       }
       // If file does not exist, create it
       else
	 f_output = fopen(filepath, "w");
	 
       fwrite(octoblock, sizeof(char), strlen(octoblock), f_output);
       printf("Wrote %d bytes to file: %s\n",strlen(octoblock), filepath);
       fclose(f_output);
     }
  }

  close(sock);
  return 0;
}
