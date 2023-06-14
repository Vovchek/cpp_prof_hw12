#pragma once

template <typename T>
class ICommand {
protected:
    virtual ~ICommand() = default;
public:
    ICommand& operator=(const ICommand& ) = delete;
    static ICommand& getInstance() {
        return T::getInstance();
    }
    virtual size_t process(char *buff, size_t length) = 0;
};
