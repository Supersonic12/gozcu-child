#include <unistd.h>

#include <iostream>
class fdguard
{
    // this class is just RAII for file descriptors. It is basic move semantics.
   public:
    explicit fdguard(int fd) : fd_(fd) { /*std::cout << "ownership transferred";*/ }
    ~fdguard()
    {
        if (fd_ > -1)
        {
            close(fd_);
        }
    };
    fdguard(const fdguard&) = delete;
    fdguard& operator=(const fdguard&) = delete;

    fdguard(fdguard&& other) noexcept : fd_{other.fd_} { other.fd_ = -1; }
    fdguard& operator=(fdguard&& other)
    {
        if (&other == this)
        {
            std::cerr << "self-assignment" << std::endl;
            return *this;
        }
        if (fd_ > -1)
        {
            close(fd_);
        }
        fd_ = other.fd_;
        other.fd_ = -1;
        return *this;
    }

    int release()
    {
        int temp = fd_;
        fd_ = -1;
        return temp;
    }

   private:
    int fd_;
};
