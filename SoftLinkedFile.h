#ifndef SOFTLINKEDFILE_H
#define SOFTLINKEDFILE_H
#include "File.h"
#include "Directory.h"

namespace GTUShell {
    class SoftLinkedFile : public File {
    public:
        SoftLinkedFile() = default;
        SoftLinkedFile(const FileData& dataVal, const shared_ptr<Directory>& dir) : File(dataVal), rootCopy(dir) { }

        iterator begin() const override;
        iterator end() const override;
        shared_ptr<File> findFile(const string& path, const GTUShell::Directory &dir) const;


        ~SoftLinkedFile() = default;
    private:
        shared_ptr<Directory> rootCopy;
    };
} //GTUShell namespace

#endif //SOFTLINKEDFILE_H