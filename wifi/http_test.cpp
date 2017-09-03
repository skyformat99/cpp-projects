#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

bool sendData (string* str_host, string* path, string* data);

/**
int main () 
{
  string host = "localhost";
  string path = "/wifi/update.php";
  string data = "name=adam-gamer&x=33.45&y=25.32";

  if (sendData(&host,&path,&data))
    return 0;
  else
    return 1;
}
**/

bool sendData (string* str_host, string* path, string* data)
{
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
    cerr << "WSAStartup failed.\n";
//    system("pause");
    return false;
  }

  SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

  struct hostent *host;
  host = gethostbyname(str_host->c_str());

  SOCKADDR_IN SockAddr;
  SockAddr.sin_port=htons(80);
  SockAddr.sin_family=AF_INET;
  SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

//  cout << "Connecting...\n";
  if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
    cerr << "Could not connect";
//    system("pause");
    return false;
  }

//  cout << "Connected.\n";
  stringstream ss;
  ss << "GET " << (*path) << "?" << (*data) << " HTTP/1.1\r\n"
     << "Host: " << (*str_host) << "\r\n"
    << "Connection: close\r\n\r\n";

  send(Socket,ss.str().c_str(), ss.str().size(),0);

/**
  char buffer[10000];
  int nDataLength;

  while ((nDataLength = recv(Socket,buffer,10000,0)) > 0){        
    int i = 0;
    while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
      cout << buffer[i];
      i += 1;
    }
  }
**/

  closesocket(Socket);
  WSACleanup();

  return true;
}