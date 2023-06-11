# Code Cờ Caro C++ trên Console

## Giới thiệu

Đây là một ứng dụng C++ đơn giản cho phép bạn chơi trò chơi Cờ Caro trên màn hình console. Ứng dụng được viết bằng ngôn ngữ C++.

## Yêu cầu hệ thống

Để biên dịch và chạy ứng dụng, bạn cần đáp ứng các yêu cầu sau:

- Phiên bản C++ 11 trở lên.
- Hệ điều hành Windows.

## Thư viện

Trong ứng dụng này, chúng ta sử dụng các thư viện sau:

```cpp
#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <windows.h>
#include <conio.h>
#include <sstream>
#include <thread>
#include <chrono>
#include <mutex>
```

- **iostream**: Để đọc và viết dữ liệu từ/to luồng nhập/xuất chuẩn.
- **string**: Để làm việc với các chuỗi ký tự.
- **iomanip**: Để điều chỉnh định dạng xuất dữ liệu.
- **cstdlib**: Để sử dụng các hàm tiện ích của thư viện chuẩn C.
- **windows.h**: Để sử dụng các hàm điều khiển màn hình trong Windows.
- **conio.h**: Để sử dụng các hàm đọc phím từ bàn phím.
- **sstream**: Để làm việc với chuỗi ký tự dưới dạng luồng.
- **thread**: Để tạo và quản lý các luồng trong ứng dụng.
- **chrono**: Để làm việc với thời gian và đo lường thời gian.
- **mutex**: Để đồng bộ hóa quá trình truy cập dữ liệu đa luồng.

## Hướng dẫn sử dụng

1. Clone repo từ GitHub:

```shell
git clone https://github.com/your_username/your_repository.git
```

2. Mở dự án bằng trình biên dịch C++ của bạn.

3. Biên dịch chương trình bằng cách chạy lệnh sau:

```shell
g++ -std=c++11 caro.cpp -o caro.exe
```

4. Chạy chương trình:

```shell
caro.exe
```

5. Khi chương trình được chạy, bạn có thể di chuyển con trỏ bằng các phím mũi tên và đánh dấu ô cờ bằng phím Space. Sau khi có người chiến thắng, bạn có thể nhấn phím Space để chơi lại.

## Tác giả

Ứng dụng này được phát triển bởi [Phạm Văn ](https://github.com/D-Jukie).

## Đóng góp

Nếu bạn muốn đóng góp vào dự án này, hãy làm theo các bước sau:

1. Fork dự án.

2. Tạo một branch mới:

```shell
git checkout -b feature/your_feature
```

3. Commit các thay đổi của bạn:

```shell
git commit -m 'Add some feature'
```

4. Push branch lên GitHub:

```shell
git push origin feature/your_feature
```

5. Tạo một yêu cầu kéo (Pull Request) mới trên GitHub.

## Bản quyền và giấy phép

[MIT License](LICENSE)
