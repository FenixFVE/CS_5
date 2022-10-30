#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>
#include <iostream>
#include <string>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

using std::string;
using std::cout;
using std::cin;

int main() {
	HANDLE hIcmpFile;
	uint64_t ipaddr = INADDR_NONE;
	DWORD dwRetVal = 0;
	string SendData;
	LPVOID ReplyBuffer = nullptr;
	DWORD ReplySize = 0;
	string SendAdr;
	string host;

	WORD sockVer;
	WSADATA wsaData;
	sockVer = MAKEWORD(2, 2);
	WSAStartup(sockVer, &wsaData);

	cout << "Enter adress: ";
	cin >> SendAdr;
	cout << "Enter data: ";
	cin >> SendData;
	cout << '\n';

	hostent* remoteHost = gethostbyname(SendAdr.c_str());
	if (remoteHost == nullptr) {
		cout << "Host does not exist";
		return 1;
	}

	host = string(inet_ntoa(*((in_addr*)remoteHost->h_addr_list[0])));
	ipaddr = inet_addr(host.c_str());

	hIcmpFile = IcmpCreateFile();

	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData.c_str());

	ReplyBuffer = new char[ReplySize];

	auto send = (char*)SendData.c_str();
	dwRetVal = IcmpSendEcho(
		hIcmpFile, 
		ipaddr, 
		send, 
		sizeof(send), 
		nullptr, 
		ReplyBuffer, 
		ReplySize, 
		1000
	);

	if (dwRetVal == 0) {
		cout << "Call to IcmpSendEcho failed.\n";
		cout << "IcmpSendEcho returned error: " << GetLastError() << '\n';
		cout << "Check if host exists";
		return 1;
	}

	auto pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
	in_addr ReplyAddr;
	ReplyAddr.S_un.S_addr = pEchoReply->Address;
	cout << "Sent echo request to " << host << '\n';
	cout << "Satutus: " << (pEchoReply->Status ? "fail" : "succes") << '\n';
	cout << "Recieved " << dwRetVal << " icmp message from " << inet_ntoa(ReplyAddr) << '\n';
	cout << "Roundtrip time = " << pEchoReply->RoundTripTime << "ms\n";
	cout << "Recived data: " << (char*)pEchoReply->Data << '\n';

	return 0;
}