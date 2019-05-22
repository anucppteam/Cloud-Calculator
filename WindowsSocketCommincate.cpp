#include <stdio.h>
#include <WinSock2.h>//윈도우 전용 소켓통신 헤더

//#include "pch.h"
//pragma message("Compiling precompiled headers.\n")
//#include "stdafx.h"
//pragma warning(disable:4996)

#pragma comment(lib, "ws2_32")//윈 소켓


#define PORT 4567//포트번호
#define PACKET_SIZE 1024//MTU와 상관없이 작은 값으로-이더넷 65000

int main() {
	WSADATA wsaData;//Windows의 소켓 초기화 정보를 저장하기위한 구조체
	WSAStartup(MAKEWORD(2, 2), &wsaData); //WSAStartup(소켓버전, WSADATA 구조체 주소)

	SOCKET hlisten;//운영체제가 관리하는 커널이라는 특수한 영역에 존재하는 기능
	hlisten = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//PF_INET6을 쓰면 IPv6를 사용하겠다는 것이다. ,SOCK_DGAM을 쓰면 UDP/IP프로토콜을 사용하겠다는 것이다.

	SOCKADDR_IN tListenAddress = {};//Windows 소켓에서 소켓을 연결할 로컬 또는 원격 주소를 지정하는 데 사용된다.
	tListenAddress.sin_family = AF_INET;//반드시 AF_INET이어야한다.
	tListenAddress.sin_port = htons(PORT);//host to network short
	tListenAddress.sin_addr.s_addr = htonl(INADDR_ANY);//host to network long, 빅엔디안-리틀엔디안과 관련이 있다.

	bind(hlisten, (SOCKADDR*)&tListenAddress, sizeof(tListenAddress));//bind(소켓, 소켓 구성요소 구조체의 주소, 그 구조체의 크기)
	listen(hlisten, SOMAXCONN);//접속 승인만 해준다.

	SOCKADDR_IN ClientAddress = {};
	int ClientSize = sizeof(ClientAddress);
	SOCKET Client = accept(hlisten, (SOCKADDR*)&ClientAddress, &ClientSize);//accept(소켓, 소켓 구성요소 주소체의 주소, 그 구조체의 크기를 담고있는 변수의 주소)

	char Buffer[PACKET_SIZE] = {};
	recv(Client, Buffer, PACKET_SIZE, 0);// recv(소켓, 수신 정보를 담을 배열주소, 그 배열의 크기, flag)-플래그는 오류 발생시 사용하는건데 안쓰므로 0으로 설정했다
	printf("Recv Message : %s\n", Buffer);

	char Message[] = "Server Send";
	send(Client, Message, strlen(Message), 0);

	closesocket(Client);
	closesocket(hlisten);

	WSACleanup(); //WSACleanup 함수는 WSAStartup 을 하면서 지정한 내용을 지워준다.
	getchar();//없으면 출력을 보기도 전에 프로그램이 꺼진다. getchar()를 통해 프로그램이 꺼지지 않도록 해준다.
	return 0;
}
