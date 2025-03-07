#include "SoftLinkedFile.h"

namespace GTUShell {
    File::iterator SoftLinkedFile::begin() const {
        string targetFilePath = getContent();

        // Find the file recursively and return the start of it
        auto filePtr = findFile(targetFilePath, *rootCopy);
        if(filePtr != nullptr) {
            return filePtr->begin();
        }
        return GTUShell::File::iterator();
    }

    File::iterator SoftLinkedFile::end() const {
        string targetFilePath = getContent();

        // Find the file recursively and return the start of it
        auto filePtr = findFile(targetFilePath, *rootCopy);
        if(filePtr != nullptr) return filePtr->end();
        return GTUShell::File::iterator();
    }

    shared_ptr<File> SoftLinkedFile::findFile(const string& pathToFind, const GTUShell::Directory &dir) const {
        char type;
        string path;
        for(const auto& filePtr : dir.getFiles()) {
            type = filePtr->getType();
            path = filePtr->getPath();
            if(pathToFind == path) {
                return filePtr;
            } else if(type == 'D' && path != "/") {
                auto subDir = std::dynamic_pointer_cast<Directory>(filePtr);
                if (subDir)
                    return findFile(pathToFind, *subDir); // Recursively search in the subdirectory
            }
        }
        return nullptr;
    }
}