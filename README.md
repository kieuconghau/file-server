# FILE SERVER

## Yêu cầu của Server
1. Một Server.
2. Tại một thời điểm, Server cho phép:
    * Nhiều Client có thể cùng login.
    * Nhiều Client có thể cùng logout.
    * Nhiều Client có thể cùng download File.
    * Chỉ một Client có thể upload 1 File. (semaphore)
3. Server lưu trữ và quản lý:
    * Danh sách các File để các Client download.
    * Danh sách các User đã được đăng ký.
    * Danh sách các Event (Log với thời điểm cụ thể) - được lưu lại trong file text.
4. Màn hình của Server hiển thị:
    * Danh sách các File để các Client download.
    * Danh sách các User (Client) đang kết nối đến (Online).
    * Log:
        - Client nào login.
        - Client nào logout (hoặc exit).
        - Client nào upload File.
        - Client nào download File.
5. Nếu Server thoát, các Client phải nhận được thông báo và logout ngay sau đó.


## Yêu cầu của Client
1. Nhiều Client.
2. Client phải đăng ký một username và password để login.
    * Kiểm tra xem username vừa đăng ký đã tồn tại hay chưa.
    * Nếu username hợp lệ thì Server bổ sung thêm User mới này.
3. Login với username và password đã đăng ký.
4. Màn hình của mỗi Client hiển thị:
    * Danh sách các File được phép download từ Server.
    * Log:
        - Client nào login.
        - Client nào logout (hoặc exit).


## Thiết kế hướng đối tượng
### Server
```cpp
class Server {
public:

private:
    mutex MutexUpload;

    SOCKET ListenSocket;
    vector<User*> OnlineUserList;
    vector<User*> UserList;
    vector<string> FileNameList;
    
    string DatabasePath;            // .../Database/
    string SharedFilesFolder;       // SharedFiles/
    string LogFile;                 // logfile.txt
    string SharedFilesNameFile;     // filename.txt
    string UsersFile;               // user.bin
    // User: USERNAMELEN | USERNAME | PWDLEN | PWD
};

struct User {
    string Username;
    string Password;
    SOCKET Socket;
};
```

### Client
```cpp
class Client {
public:

private:
    string DatabasePath;            // .../Database/
    string LogFile;                 // logfile.txt
};
```

## Cấu trúc của một thông điệp (gói tin)
### Client --> Server
`FLAG` | `MSGLEN` | `MSG`
Trong đó:
* `FLAG` (uint8_t - enum class): Kiểu thông điệp.
    - 0x00: Register (username)
    - 0x01: Login (username)
    - 0x02: Password
    - 0x03: Upload File
    
    - 0x04: Download File Request (index)
    - 0x05: Logout  
    (Crash: SOCKET_ERROR)
* `MSGLEN` (uint64_t): Chiều dài của MSG tính theo byte.
* `MSG` (char*): Nội dung của thông điệp.

### Server --> Client
`FLAG` | `MSGLEN` | `MSG`
* `FLAG` (uint8_t - enum class): Kiểu thông điệp.
    - 0x00: Fail
    - 0x01: Success
    
    - 0x02: Download File Data (file data)
    - 0x03: Logout  
    (Crash: SOCKET_ERROR)
* `MSGLEN` (uint64_t): Chiều dài của MSG tính theo byte.
* `MSG` (char*): Nội dung của thông điệp.

## Kịch bản trao đổi
### Thiết lập kết nối giữa Client và Server
1. Server (`ListenSocket`) listen, đợi kết nối từ các Client (`ConnectSocket`).
2. Server tạo ra một socket mới là `AcceptSocket`.
3. Client connect tới Server.
4. Server accept và thiết lập đường truyền giữa `AcceptSocket` và `ConnectSocket`. Quá trình trao đổi dữ liệu trên đường truyền vừa thiết lập được bỏ vào một Thread độc lập, dẫn tới Server sẽ tiếp tục listen và đợi kết nối từ các Client khác.

### Register
1. Nhập username mà Client muốn đăng ký.
2. Send gói tin có chứa thông tin của username này cho Server để kiểm tra username này có tồn tại hay chưa theo cú pháp:
    * Nếu tồn tại,...
    * Nếu không tồn tại,...

### Login


### Download File


### Upload File (semaphore)


### Thông báo


### Logout


### Exit


### Hủy kết nối giữa Client và Server


## Tính năng
1. Cho phép nhiều Client cùng login với một username.


## Code mẫu
### Mutex & Thread
```cpp
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

std::mutex mtx;           // mutex for critical section

void print_block(int n, char c) {
	// critical section (exclusive access to std::cout signaled by locking mtx):
	mtx.lock();
	for (int i = 0; i < n; ++i) { std::cout << c; }
	std::cout << '\n';
	mtx.unlock();
}

int main()
{
	std::thread th1(print_block, 50, '*');
	std::thread th2(print_block, 50, '$');

	th1.join();
	th2.join();

	return 0;
}
```
### Simple Chat (One Server - Multiple Clients)
#### Server
```cpp
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"

SOCKET AcceptSocketList[10];
size_t SocketCount = 0;

DWORD WINAPI sendMsg(LPVOID acceptSocket) {
	string msg;
	size_t msgLen;

	while (true) {
		getline(cin, msg);

		msg = "Server: " + msg;
		msgLen = msg.length();

		for (size_t i = 0; i < SocketCount; ++i) {
			send(AcceptSocketList[i], (char*)&msgLen, sizeof(msgLen), 0);
			send(AcceptSocketList[i], msg.c_str(), msg.length(), 0);
		}
	}

	return 0;
}

DWORD WINAPI rcvMsg(LPVOID acceptSocket) {
	char* msg;
	size_t msgLen;

	SOCKET sk_value = *(SOCKET*)acceptSocket;

	while (true) {
		recv(*((SOCKET*)acceptSocket), (char*)&msgLen, sizeof(msgLen), 0);
		msg = new char[msgLen + 1];
		msg[msgLen] = '\0';
		recv(*((SOCKET*)acceptSocket), msg, msgLen, 0);

		cout << msg << "\n";

		for (size_t i = 0; i < SocketCount; ++i) {
			if (AcceptSocketList[i] != *((SOCKET*)acceptSocket)) {
				send(AcceptSocketList[i], (char*)&msgLen, sizeof(msgLen), 0);
				send(AcceptSocketList[i], msg, msgLen, 0);
			}
		}

		delete[] msg;
	}

	return 0;
}

DWORD WINAPI sendAndRcvMsg(LPVOID acceptSocket) {
	HANDLE sendMsgThread;
	HANDLE rcvMsgThread;

	sendMsgThread = CreateThread(NULL, 0, sendMsg, (SOCKET*)acceptSocket, 0, NULL);
	rcvMsgThread = CreateThread(NULL, 0, rcvMsg, (SOCKET*)acceptSocket, 0, NULL);

	while (true);

	// Discard this Accept socket from the AcceptSocketList
	// TODO: code here

	// Close the Accept socket
	if (shutdown(*((SOCKET*)acceptSocket), SD_SEND) == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
	}

	closesocket(*((SOCKET*)acceptSocket));

	return 0;
}

int main() {
	WSAData wsaData;

	int iResult;

	// Initialize Winsock
	WORD ver = MAKEWORD(2, 2);					// version 2.2
	iResult = WSAStartup(ver, &wsaData);		// initiate use of WS2_32.dll

	if (iResult != 0) {
		printf("WSAStartup() failed: %d\n", iResult);
		return 1;
	}

	// Create a socket for the server
	ADDRINFOA* result = NULL;
	ADDRINFOA hints;

	ZeroMemory(&hints, sizeof(hints));		// Fill a block of memory (hints) with zeros
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);		// result is the pointer pointing to the first ADDRINFO in the linked list

	if (iResult != 0) {
		printf("getaddrinfo() failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Bind the socket which has already been created (ListenSocket) to an IP address and port
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		printf("bind() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	// Listen on a socket
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("listen() failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept connections from clients
	while (true) {
		SOCKET AcceptSocket = INVALID_SOCKET;

		AcceptSocket = accept(ListenSocket, NULL, NULL);

		if (AcceptSocket == INVALID_SOCKET) {
			printf("accept() failed: %d\n", WSAGetLastError());
			continue;
		}

		AcceptSocketList[SocketCount] = AcceptSocket;

		HANDLE sendAndRcvMsgThread = CreateThread(NULL, 0, sendAndRcvMsg, &AcceptSocketList[SocketCount], 0, NULL);

		++SocketCount;
	}

	// Close the Listen socket
	closesocket(ListenSocket);

	// Clean up
	WSACleanup();

	return 0;
}
```

#### Client
```cpp
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_IP "127.0.0.1"

bool isActive = true;

DWORD WINAPI sendMsg(LPVOID connectSocket) {
	string msg;
	size_t msgLen;

	while (true) {
		getline(cin, msg);

		msg = "Client: " + msg;
        msgLen = msg.length();

		send(*((SOCKET*)connectSocket), (char*)&msgLen, sizeof(msgLen), 0);
		send(*((SOCKET*)connectSocket), msg.c_str(), msgLen, 0);
    }

    return 0;
}

DWORD WINAPI rcvMsg(LPVOID connectSocket) {
	char* msg;
	size_t lenMsg;

	while (true) {
		recv(*((SOCKET*)connectSocket), (char*)&lenMsg, sizeof(lenMsg), 0);
		msg = new char[lenMsg + 1];
		msg[lenMsg] = '\0';
		recv(*((SOCKET*)connectSocket), msg, lenMsg, 0);

		cout << msg << "\n";

        delete[] msg;
    }

    return 0;
}

int main() {
	WSAData wsaData;

	int iResult;

	// Initialize Winsock
	WORD ver = MAKEWORD(2, 2);		// version 2.2
	iResult = WSAStartup(ver, &wsaData);		// initiate use of WS2_32.dll

	if (iResult != 0) {
		printf("WSAStartup() failed: %d\n", iResult);
		return 1;
	}

	// Create a socket for the server
	ADDRINFOA* result = NULL;
	ADDRINFOA hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);		// result is the pointer pointing to the first ADDRINFO in the linked list

	if (iResult != 0) {
		printf("getaddrinfo() failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;

	// Attempt to connect to an address until one succeeds
	for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to the Server!\n");
		WSACleanup();
		return 1;
	}

	HANDLE sendMsgThread;
	HANDLE rcvMsgThread;

	sendMsgThread = CreateThread(NULL, 0, sendMsg, &ConnectSocket, 0, NULL);
	rcvMsgThread = CreateThread(NULL, 0, rcvMsg, &ConnectSocket, 0, NULL);

	while (true);

	// Close the Connect socket
	iResult = shutdown(ConnectSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	closesocket(ConnectSocket);

	// Clean up
	WSACleanup();

	return 0;
}
```
