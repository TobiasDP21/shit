#pragma once

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace UnityReflection {

class IPCClient {
public:
    using DataCallback = std::function<void(const std::string& data)>;
    using ErrorCallback = std::function<void(const std::string& error)>;

    IPCClient();
    ~IPCClient();

    void SetDataCallback(DataCallback callback);
    void SetErrorCallback(ErrorCallback callback);

    bool Connect();
    void Disconnect();
    bool IsConnected() const;

    void StartListening();
    void StopListening();

private:
    void ListenThread();
    std::string ReadData();

    DataCallback dataCallback_;
    ErrorCallback errorCallback_;

    std::atomic<bool> isConnected_{false};
    std::atomic<bool> isListening_{false};
    std::unique_ptr<std::thread> listenThread_;

#ifdef _WIN32
    HANDLE hPipe_ = INVALID_HANDLE_VALUE;
    static constexpr const char* PIPE_NAME = "\\\\.\\pipe\\UnityReflectionPipe";
#else
    int fd_ = -1;
    static constexpr const char* PIPE_NAME = "/tmp/UnityReflectionPipe";
#endif
};

} // namespace UnityReflection
