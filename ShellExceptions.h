#include <stdexcept>
#include <string>

class ShellExceptions : public std::runtime_error {
    public:
    explicit ShellExceptions(const std::string& what) : std::runtime_error(what) { }

};

class FileNotFound : public ShellExceptions {
    public:
    explicit FileNotFound(const std::string& filename) : ShellExceptions("No such file or directory: " + filename) { }
};


class FileIsDirectory : public ShellExceptions {
    public:
    explicit FileIsDirectory(const std::string& filename) : ShellExceptions(filename + ": Is a directory") { }

};

class ContentsFileNotFound : public ShellExceptions {
    public:
    ContentsFileNotFound() : ShellExceptions("Program terminated: 'disk.txt' was not found. Attempted to create it, you can start the program again.\n") { }
};

class FileTypeInvalid : public ShellExceptions {
    public:
    FileTypeInvalid() : ShellExceptions("Program terminated: invalid file type detected on 'disk.txt' file.\nYou may delete disk.txt and start the program again.\n") { }
};

class DirectoryAlreadyExists : public ShellExceptions {
public:
    explicit DirectoryAlreadyExists(const std::string& filename) : ShellExceptions("File exists: " + filename) { }
};

class PathNotFound : public ShellExceptions {
public:
    explicit PathNotFound(const std::string& filename) : ShellExceptions("File not found: " + filename) { }
};

class NotDirectory : public ShellExceptions {
public:
    explicit NotDirectory(const std::string& filename) : ShellExceptions("Not a directory: " + filename) { }
};

class DirectoryNotEmpty : public ShellExceptions {
public:
    explicit DirectoryNotEmpty(const std::string& filename) : ShellExceptions("Directory is not empty: " + filename) { }
};



class DiskExceedsLimit : public ShellExceptions {
public:
    explicit DiskExceedsLimit() : ShellExceptions("Disk file exceed the limit of 10MB, terminating the system...\n\n") { }
};