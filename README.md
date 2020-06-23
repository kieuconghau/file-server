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
    semaphore MutexUpload = 1;

    // block(MutexUpload);
    // // Critical Section
    // unblock(MutexUpload);

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
}

struct User {
    string Username;
    string Password;
    SOCKET Socket;
}
```
### Client
```cpp
class Client {
public:

private:
    string DatabasePath;            // .../Database/
    string LogFile;                 // logfile.txt
}
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

### Đăng ký username
1. Nhập username mà Client muốn đăng ký.
2. Send gói tin có chứa thông tin của username này cho Server để kiểm tra username này có tồn tại hay chưa theo cú pháp:
    * Nếu tồn tại,...
    * Nếu không tồn tại,...

### Login


### Download File


### Upload File


### Thông báo


### Logout


### Exit


### Hủy kết nối giữa Client và Server


## Tính năng
1. Cho phép nhiều Client cùng login với một username.