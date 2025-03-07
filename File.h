#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <iterator>
#include <chrono>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <memory>

#include "ShellExceptions.h"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::shared_ptr;


namespace GTUShell {
    // A struct to hold the information about a file
    struct FileData {
        char type;
        string name;
        string path;
        string date;
        int size;
        string content;
    };

    class File {
    public:
        File() = default;
        explicit File(FileData dataVal) : data(std::move(dataVal)) { }

        virtual void cat() const;

        // Pure virtual iterator function definitions
        using iterator = std::string::const_iterator;
        virtual iterator begin() const = 0;
        virtual iterator end() const = 0;

        // Setters and getters for the class
        void setData(const FileData& newData);
        char getType() const;
        string getPath() const;
        string getName() const;
        string getContent() const;
        string getDate() const;
        int getSize() const;

        static void checkDiskSize();

        virtual ~File() = default;

    protected:
        FileData data;
    };

} // GTUShell namespace


#endif //FILE_H
