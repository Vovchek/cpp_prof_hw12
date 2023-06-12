#pragma once

template <typename T>
class ICommand {
protected:
    virtual ~ICommand() = default;
public:
    virtual void Delete() = 0;
    ICommand& operator=(const ICommand& ) = delete;
    static ICommand* createInstance() {
        return new T();
    }
    virtual size_t process(char *buff, size_t length) = 0;
};
