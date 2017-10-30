#include <stdio.h>  //std i/o
#include <stdlib.h> 
#include <netdb.h>  
#include <netinet/in.h>
#include <unistd.h> 
#include <string.h> //string
#define YourPORT 5001
//function for reading from socket 
int readn(int sockfd, char *buf, int n){           
    int k;  //reader
    int off = 0;    //do konca
    for(int i = 0; i < n; ++i){
        k = read(sockfd, buf + off, 1);
        off += 1;
        if (k < 0){
            printf("Error reading from socket \n");
            exit(1);
        }
    }
    return off;
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd; // socket descriptors
    uint16_t portno;       //port number default 5001
    unsigned int clilen;    // razmer IP client'a
    char buffer[256];      
    char *p = buffer;
    struct sockaddr_in serv_addr, cli_addr; //addresa socketov
    ssize_t n;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);   //sozdajom socket TCP, for UDP use SOCK_DGRAM

    if (sockfd < 0) {                       //obrabotka oshibki otkritiya socketa
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = YourPORT; 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here process will
       * go in sleep mode and will wait for the incoming connection
    */
    
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    /* Accept actual connection from the client */
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    shutdown(sockfd, 2);    //cheknuly, ne zabivaem pribrat za soboy
    close(sockfd);
    
    if (newsockfd < 0) {        //esli ne cheknulsya
        perror("ERROR on accept");
        exit(1);
    }

    /* If connection is established then start communicating */
    bzero(buffer, 256);     //obnulyaem buffer
    
    n = readn(newsockfd, p, 255);   //nachinaem chitat iz socket'a

    printf("Your's message: %s\n", buffer);     //poluchaem messag'u

    /* Write a response to the client */
    n = write(newsockfd, "Message get!", 255); // send on Client

    if (n < 0) {                //esli client umer, ili nepoladki s setju
        perror("ERROR writing to socket");
        exit(1);
    }

    shutdown(newsockfd, 2);     //vikluchaem potok
    close(newsockfd);       //close socket

    return 0;
}