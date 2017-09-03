#include "stdio.h"
#include <time.h>
#include <winsock.h>

#define MAIL_HOST "www.adziorny.org"
#define MAIL_PORT 2525

#define DEBUG false
#define DEBUG_2 true

/** Send-receive function */
int sendAndReceive (SOCKET, const char*, int, const char*);

/**
 * Main function executed in this file.
 */
int main (int argc, char* argv[]) {

  // Required for Windows Socket initialization
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
    printf("WSAStartup failed.\n");
    exit(1);
  }

  // Get the host IP, either this computer or from the argv[1]
  struct hostent *localIP;
  char ipBuffer[256];
  if (argc > 1)
    localIP = gethostbyname(argv[1]);
  else
    localIP = gethostbyname("ael-audlab");
  if (localIP == NULL) { // do some error checking
    printf("Error: Gethostbyname for localIP");
    exit(1);
  }
  if (DEBUG_2) printf("Local IP address: %s\n", 
    inet_ntoa(*(struct in_addr*)localIP->h_addr));

  // And store in buffer
  sprintf_s(ipBuffer,_countof(ipBuffer),
    "New AAE Server IP Address: %s\r\n",
    inet_ntoa(*(struct in_addr*)localIP->h_addr));

  // Now get the address of the mail host we're using
  struct hostent *mailHost;
  mailHost = gethostbyname(MAIL_HOST);
  if (mailHost == NULL) { // do some error checking
    printf("Error on gethostbyname!");
    exit(1);
  }

  // Set up a socket address structure
  struct sockaddr_in saSender;
  saSender.sin_family = AF_INET;
  saSender.sin_addr.s_addr = ((struct in_addr*)mailHost->h_addr)->s_addr;
  saSender.sin_port = htons(MAIL_PORT);

  // And set up the socket
  SOCKET sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

  // Connect to the socket
  int rVal, errorCode;
  rVal = connect(sock, (struct sockaddr *)&saSender, sizeof(saSender));

  // If we couldn't connect to socket, display the error-code
  if (rVal != 0) {

    errorCode = WSAGetLastError();
    printf("Error connecting: %d\n",errorCode);

  // Otherwise, start the send procedure
  } else {

    char buffer[256];
    char dBuf[10];
    char tBuf[11];

    if (DEBUG) printf("Socket connected (%d)\n",rVal);

    rVal = 1;

    // Send SMTP protocol intro
    rVal *= sendAndReceive(sock,"HELO\n",2,"220");
    rVal *= sendAndReceive(sock,"MAIL FROM: adam_dziorny@urmc.rochester.edu\n",1,"250");
    rVal *= sendAndReceive(sock,"RCPT TO: ual@adziorny.org\n",1,"250"); 
    rVal *= sendAndReceive(sock,"DATA\n",1,"354");

    // Send Email Headers
    rVal *= sendAndReceive(sock,"From: AAE Server <adam_dziorny@urmc.rochester.edu>\r\n",0,"");
    rVal *= sendAndReceive(sock,"To: Adam Dziorny <ual@adziorny.org>\r\n",0,"");
    rVal *= sendAndReceive(sock,"Subject: AAE Server IP Changed\r\n",0,"");

    // Send body
    rVal *= sendAndReceive(sock,ipBuffer,0,"");
    _strdate_s(dBuf,_countof(dBuf));
    _strtime_s(tBuf,_countof(tBuf));
    sprintf_s(buffer,_countof(buffer),"Date: %s Time: %s\r\n",dBuf,tBuf);  
    rVal *= sendAndReceive(sock,buffer,0,"");

    // Close up
    rVal *= sendAndReceive(sock,"\r\n.\r\n",1,"250");
    rVal *= sendAndReceive(sock,"QUIT\n",1,"221");

    if (DEBUG_2 && rVal != 1)
      printf("Error on send-recieves!\n");
 
    closesocket(sock);
  }



  // Cleanup windows socket
  WSACleanup();

  exit(0);
}

/**
 * Perform a send and receive on the given socket.
 */
int sendAndReceive (SOCKET sock, const char* msg, int numRcvs, const char* cmpStr) {

  if (numRcvs > 0 && strlen(cmpStr) != 3) {
    printf("Error: comparison string is of invalid size!\n");
    return 1;
  }

  int i, bytes_sent, bytes_recv, returnVal = 0;
  char buffer[256];
  char m_code[4];

  // If there are no return vals to grab, then set returnVal == 1
  if (numRcvs == 0) 
    returnVal = 1;

  // Send the message
  bytes_sent = send(sock, msg, strlen(msg), 0);
  if (DEBUG) printf("Bytes sent: %d\n",bytes_sent);

  // Loop for a given number of receives
  for (i=0;i<numRcvs;i++) {
    bytes_recv = recv(sock, buffer, 256, 0);
    if (DEBUG) printf("Bytes recv: %d\n",bytes_recv);

    if (bytes_recv > 0) {
      buffer[bytes_recv] = '\0';
      if (DEBUG) printf("  Message recv: %s",buffer);
    }

    if (bytes_recv > 3) {
      strncpy_s(m_code,_countof(m_code),buffer,3);
      if (strcmp(cmpStr,m_code)==0)
        returnVal = 1;
    }

    // If we're getting '0' back, we've lost connection
    if (bytes_recv == 0)
      returnVal = 2;
  }

  return returnVal;
}