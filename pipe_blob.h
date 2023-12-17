#ifndef PIPE_BLOB_H_
#define PIPE_BLOB_H_

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>

class PipeDataSharing {
public:
    PipeDataSharing(const std::string &pipeName);
    ~PipeDataSharing();

    bool CreatePipe();
    bool WriteToPipe(const std::string &message);
    std::string ReadFromPipe();

private:
    std::string pipeName_;
    int pipeFd_[2];
};

#endif  // PIPE_BLOB_H_
