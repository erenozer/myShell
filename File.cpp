#include "File.h"

namespace GTUShell {

    string File::getName() const {
        return data.name;
    }

    void File::setData(const FileData &newData) {
        data = newData;
    }

    char File::getType() const {
        return data.type;
    }

    string File::getPath() const {
        return data.path;
    }

    string File::getContent() const {
        return data.content;
    }

    string File::getDate() const {
        return data.date;
    }

    int File::getSize() const {
        return data.size;
    }

    void File::cat() const {
        auto it = begin();
        auto endIt = end();

        // Loop through the File using their iterators
        for (it; it != endIt; ++it) {
            cout << *it;
        }

        cout << "\n";
    }

    void File::checkDiskSize() {
        ifstream inputStream("disk.txt", std::ios::binary);

        if (!inputStream.is_open()) {
            return;
        }

        // Move file pointer to the end
        inputStream.seekg(0, std::ios::end);

        // Get the file by getting the position of file pointer
        size_t fileSize = inputStream.tellg();

        // Check if the file exceeds 10MB
        size_t maxFileSize = 10*1024*1024;
        if (fileSize > maxFileSize) {
            throw DiskExceedsLimit();
        }

        inputStream.close();
    }
}