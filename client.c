#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdbool.h>
#define EXAMPLE_PORT 6000
#define EXAMPLE_GROUP "239.0.0.1"
#define BUFFSIZE 255
void Die(char *mess) { perror(mess); exit(1); }
short multicast()
{
   struct sockaddr_in addr;
   int addrlen, sock, cnt;
   struct ip_mreq mreq;
   short message;
   bool flag = false;
   /* set up socket */
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) {
     perror("socket");
     exit(1);
   }
   
   bzero((char *)&addr, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_port = htons(EXAMPLE_PORT);
   addrlen = sizeof(addr);
    /* receive */
      if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {        
         perror("bind");
	 exit(1);
      }    
      mreq.imr_multiaddr.s_addr = inet_addr(EXAMPLE_GROUP);         
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
      if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		     &mreq, sizeof(mreq)) < 0) {
	 perror("setsockopt mreq");
	 exit(1);
      }         
      
 	 cnt = recvfrom(sock, &message, sizeof(message), 0, 
			(struct sockaddr *) &addr, &addrlen);
	 if (cnt < 0) {
	    perror("recvfrom");
	    exit(1);
	 } else if (cnt == 0) {
 	    
	 }
	 printf("%s: message = \"%d\"\n", inet_ntoa(addr.sin_addr), message);
	 
	 
        
return message;

}
int main(int argc, char *argv[]) {

            int sock;
	    short port;
            struct sockaddr_in echoserver;
            struct sockaddr_in echoclient;
            char buffer[BUFFSIZE];
            unsigned int echolen, clientlen;
            int received = 0;

            if (argc != 3) {
              fprintf(stderr, "USAGE: %s <server_ip> <word> \n", argv[0]);
              exit(1);
            }

	   port = multicast();
          /* Create the UDP socket */
            if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
              Die("Failed to create socket");
            }
            /* Construct the server sockaddr_in structure */
            memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
            echoserver.sin_family = AF_INET;                  /* Internet/IP */
            echoserver.sin_addr.s_addr = inet_addr(argv[1]);  /* IP address */
            echoserver.sin_port = port;       /* server port */


	    /* Send the word to the server */
            echolen = strlen(argv[2]);
            if (sendto(sock, argv[2], echolen, MSG_DONTWAIT,
                       (struct sockaddr *) &echoserver,
                       sizeof(echoserver)) != echolen) {
              Die("Mismatch in number of sent bytes");
            }

	    /* Receive the word back from the server */
            fprintf(stdout, "Received: ");
            clientlen = sizeof(echoclient);
            if ((received = recvfrom(sock, buffer, BUFFSIZE, 0,
                                     (struct sockaddr *) &echoclient,
                                     &clientlen)) != echolen) {
              Die("Mismatch in number of received bytes");
            }
            /* Check that client and server are using same socket */
            if (echoserver.sin_addr.s_addr != echoclient.sin_addr.s_addr) {
              Die("Received a packet from an unexpected server");
            }
            buffer[received] = '\0';        /* Assure null-terminated string */
            fprintf(stdout, buffer);
            fprintf(stdout, "\n");
            close(sock);
            exit(0);
          }


