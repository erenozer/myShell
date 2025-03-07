#ifndef REGULARFILE_H
#define REGULARFILE_H

#include "File.h"

namespace GTUShell {
    class RegularFile : public File {
    public:
        RegularFile() = default;
        explicit RegularFile(const FileData& dataVal) : File(dataVal) { }

        // Iterator overrides
        iterator begin() const override;
        iterator end() const override;

        ~RegularFile() = default;
    };
} //GTUShell namespace

#endif //REGULARFILE_H
