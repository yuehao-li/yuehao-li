/*
 * Copyright (C) 2018-2022 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 */

#include <iostream>
#include <cstring>
#include <string>
#include <cmath>
#include <functional>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <cerrno>
#include <unistd.h>
#include <crypt.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define CORE 24
#define MULITI_CORE 96
#define PASSWORD_LENGTH 4
#include "cracker.h" 
using namespace std;
mutex m;


typedef struct Internal{
    //Indicator condition = NOT_FOUND;
    char passwd[PASSWORD_LENGTH+1];
}Internal;

void sendMulti(char *passwd){
    int tt1;
    int socksend = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(socksend, SOL_SOCKET, SO_REUSEADDR, &tt1, sizeof(int));
    setsockopt(socksend, SOL_SOCKET, SO_REUSEPORT, &tt1, sizeof(int));
    setsockopt(socksend, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &tt1, sizeof(int));
    struct sockaddr_in server_addr_recv;
    bzero((char *) &server_addr_recv, sizeof(server_addr_recv));
    server_addr_recv.sin_family = AF_INET;
    server_addr_recv.sin_port = htons(get_multicast_port());
    server_addr_recv.sin_addr.s_addr = get_multicast_address();
    sendto(socksend, passwd, sizeof(passwd), 0, (struct sockaddr *) &server_addr_recv, sizeof(server_addr_recv));

    close (socksend);

}

int receiveUDP (Message &msg){ //, int index

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    
    if(sockfd < 0) exit(-1);
    struct sockaddr_in server_addr;    
    bzero((char *) &server_addr, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = htons(get_multicast_port()); 
    bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    struct ip_mreq multicastRequest;
    multicastRequest.imr_multiaddr.s_addr = get_multicast_address();  //inet_addr() ????
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &multicastRequest, sizeof(multicastRequest)) < 0)
        exit(-1);\
    recvfrom(sockfd, &msg, sizeof(msg), 0, NULL, 0);
    return(sockfd);
}

void sendback(Message &msg){
    int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server = gethostbyname(msg.hostname);
    struct sockaddr_in server_addr2;
    memset(&server_addr2, 0, sizeof(server_addr2));
    server_addr2.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr2.sin_addr.s_addr, server->h_length);
    server_addr2.sin_port = msg.port;
    int connectTCP = connect(sockfd2, (struct sockaddr *) &server_addr2, sizeof(server_addr2));
    if(connectTCP < 0) perror("Error");
    send(sockfd2, &msg, sizeof(msg), 0);
    close(sockfd2);
}

void parallelCrack(char *alphabet,  int index, atomic<bool> &is_found, //int socksend, struct sockaddr_in server_addr_recv,  
                    char cominghash[MAX_HASHES][HASH_LENGTH+1],int numpasswd){  //Indicator &condition, //,  //Message &msg, atomic<int> &numFind,

    int len = strlen(alphabet);
    int len2 = len*len;
    int len3 = len*len*len;
    struct crypt_data data;
    char salt[3];
    char hash[15];
    char passTemp[5];
    int totalPossible = pow(len, PASSWORD_LENGTH);
    for(int hashIndex = 0; hashIndex < numpasswd; hashIndex++){ 
        is_found = false;
        strcpy(hash, cominghash[hashIndex]);
        salt[0] = hash[0];
        salt[1] = hash[1];
        salt[2] = '\0';
        //char hashTemp[20];
        int indicator = index;
        int i=0, j=0 , k=0, l=0;
        
        //Internal internal;
        data.initialized = 0;
        while (indicator <= totalPossible && strlen(cominghash[hashIndex]) == HASH_LENGTH){
        
            i = indicator % len;
            j = indicator / (len) % len;
            k = indicator / len2 % len;
            l = indicator / len3 % len;
            passTemp[0] = alphabet[l];
            passTemp[1] = alphabet[k];
            passTemp[2] = alphabet[j];
            passTemp[3] = alphabet[i];
            passTemp[4] = '\0';
            char* hashTemp = crypt_r(passTemp, salt, &(data));
            if(strcmp(hash, hashTemp) == 0){ //strcmp(hash, crypt(passTemp, salt)) == 0
                sendMulti(passTemp);

            }
            indicator += 96;
        }
    }
}

void main_listener(atomic<bool> &is_found,  int &sockrecv, char password[MAX_HASHES][HASH_LENGTH+1], atomic<int>&findnum, 
                    int numpass){ //int socksend,Message &msg,char*host , int numHash, , atomic<int> &numFind , int numhash

     while (findnum < numpass){ //findnum.load() != numhash

        char passtemp[5];
        recvfrom(sockrecv, &passtemp, sizeof(passtemp), 0, NULL, 0);
        is_found = true;
        strcpy(password[findnum.load()], passtemp);
        findnum++;
        
     }


}

// Modified cracker function
void Modified_crack(Message &msg, char* host, int &sockrecv){
    int numPassword = ntohl(msg.num_passwds);
    //char password[numPassword][5];
    // for(int hashIndex = 0; hashIndex < numPassword; hashIndex++){
        //cout << "cracking " <<msg.passwds[hashIndex] << endl;
        vector <thread> threads;
        atomic<bool> is_found  {false};
        atomic<int> numFind  {0};
        
        int i, j;
        // char password[5];
        if(strcmp(host, "olaf") == 0){
            i = 0;
            j = 24;
        }
        else if(strcmp(host, "thor") == 0){
            i = 24;
            j = 48;
        }
        else if(strcmp(host, "nogbad") == 0){
            i = 48;
            j = 72;
        }
        else if(strcmp(host, "noggin") == 0){
            i = 72; //72
            j = 96; //96
        }
        for (; i < j; i++){
            if(i == 0 || i == 24 || i == 48 || i == 72){ 
                threads.push_back(thread(main_listener, 
                                    ref(is_found),  ref(sockrecv), 
                                    ref(msg.passwds), ref(numFind), 
                                    numPassword)); //socksend,ref(msg),,  host, numPassword , ref(numFind)
            }
            threads.push_back(thread(parallelCrack, msg.alphabet, i, ref(is_found), 
                                    ref(msg.passwds), numPassword)); //(password[hashIndex]),//numPassword, //ref(msg), ref(numFind),
        }
        for(long unsigned int i = 0; i < threads.size(); i++){
            if(i == 0){
                threads[i].join();
            }
            else{
                threads[i].join();
            }
        }
    
}

int main() { //int argc, char *argv[]
    Message msg;
    char hostname[10]; 
    gethostname(hostname, sizeof(hostname));
        int sockrecv;
        cout << "Waiting from "<< hostname << endl;
        sockrecv = receiveUDP(msg);
        cout<<"received" << endl;
        Modified_crack(msg, hostname,  sockrecv);

        // ================================================================================
        //                             Olaf Send Back
        // ================================================================================

        if (strcmp(hostname, "olaf") == 0) {
            sendback(msg);
            cout << hostname <<": send finish." << endl;
        }
        close(sockrecv);
    //}
}