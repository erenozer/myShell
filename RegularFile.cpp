#include "RegularFile.h"

namespace GTUShell {
    File::iterator RegularFile::begin() const {
        return data.content.begin();
    }

    File::iterator RegularFile::end() const {
        return data.content.end();
    }


} // GTUShell namespace
