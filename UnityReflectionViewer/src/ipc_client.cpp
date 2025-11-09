#include "ipc_client.h"
#include <iostream>
#include <cstring>

namespace UnityReflection {

IPCClient::IPCClient() {
}

IPCClient::~IPCClient() {
    StopListening();
    Disconnect();
}

void IPCClient::SetDataCallback(DataCallback callback) {
    dataCallback_ = callback;
}

void IPCClient::SetErrorCallback(ErrorCallback callback) {
    errorCallback_ = callback;
}

bool IPCClient::Connect() {
#ifdef _WIN32
    hPipe_ = CreateFileA(
        PIPE_NAME,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hPipe_ == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        if (errorCallback_) {
            errorCallback_("Failed to connect to pipe. Error: " + std::to_string(error));
        }
        return false;
    }

    DWORD mode = PIPE_READMODE_BYTE;
    if (!SetNamedPipeHandleState(hPipe_, &mode, NULL, NULL)) {
        if (errorCallback_) {
            errorCallback_("Failed to set pipe mode");
        }
        CloseHandle(hPipe_);
        hPipe_ = INVALID_HANDLE_VALUE;
        return false;
    }
#else
    fd_ = open(PIPE_NAME, O_RDONLY);
    if (fd_ == -1) {
        if (errorCallback_) {
            errorCallback_("Failed to open named pipe: " + std::string(strerror(errno)));
        }
        return false;
    }
#endif

    isConnected_ = true;
    return true;
}

void IPCClient::Disconnect() {
#ifdef _WIN32
    if (hPipe_ != INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe_);
        hPipe_ = INVALID_HANDLE_VALUE;
    }
#else
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
#endif
    isConnected_ = false;
}

bool IPCClient::IsConnected() const {
    return isConnected_;
}

void IPCClient::StartListening() {
    if (isListening_) return;

    isListening_ = true;
    listenThread_ = std::make_unique<std::thread>(&IPCClient::ListenThread, this);
}

void IPCClient::StopListening() {
    isListening_ = false;
    if (listenThread_ && listenThread_->joinable()) {
        listenThread_->join();
    }
}

void IPCClient::ListenThread() {
    while (isListening_) {
        if (!IsConnected()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        std::string data = ReadData();
        if (!data.empty() && dataCallback_) {
            dataCallback_(data);
        }

        Disconnect();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

std::string IPCClient::ReadData() {
#ifdef _WIN32
    // Read length prefix (4 bytes)
    DWORD bytesRead = 0;
    int32_t dataLength = 0;

    if (!ReadFile(hPipe_, &dataLength, sizeof(dataLength), &bytesRead, NULL) ||
        bytesRead != sizeof(dataLength)) {
        if (errorCallback_) {
            errorCallback_("Failed to read data length");
        }
        return "";
    }

    if (dataLength <= 0 || dataLength > 100 * 1024 * 1024) { // Max 100MB
        if (errorCallback_) {
            errorCallback_("Invalid data length: " + std::to_string(dataLength));
        }
        return "";
    }

    // Read data
    std::string data;
    data.resize(dataLength);

    DWORD totalRead = 0;
    while (totalRead < static_cast<DWORD>(dataLength)) {
        DWORD toRead = dataLength - totalRead;
        if (!ReadFile(hPipe_, &data[totalRead], toRead, &bytesRead, NULL)) {
            if (errorCallback_) {
                errorCallback_("Failed to read data");
            }
            return "";
        }
        totalRead += bytesRead;
    }

    return data;
#else
    // Read length prefix (4 bytes)
    int32_t dataLength = 0;
    ssize_t bytesRead = read(fd_, &dataLength, sizeof(dataLength));

    if (bytesRead != sizeof(dataLength)) {
        if (errorCallback_) {
            errorCallback_("Failed to read data length");
        }
        return "";
    }

    if (dataLength <= 0 || dataLength > 100 * 1024 * 1024) { // Max 100MB
        if (errorCallback_) {
            errorCallback_("Invalid data length: " + std::to_string(dataLength));
        }
        return "";
    }

    // Read data
    std::string data;
    data.resize(dataLength);

    ssize_t totalRead = 0;
    while (totalRead < dataLength) {
        ssize_t n = read(fd_, &data[totalRead], dataLength - totalRead);
        if (n <= 0) {
            if (errorCallback_) {
                errorCallback_("Failed to read data");
            }
            return "";
        }
        totalRead += n;
    }

    return data;
#endif
}

} // namespace UnityReflection
