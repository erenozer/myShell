#ifndef DIRECTORY_H
#define DIRECTORY_H
#include "File.h"
#include <map>

namespace GTUShell {
    class Directory : public File {
    public:
        Directory() = default;

        // Iterator overrides
        iterator begin() const override;
        iterator end() const override;

        // Getter function for the files vector
        vector<shared_ptr<File> > getFiles() const;

        // Function for managing the file system
        static void cd(string& currentPath, const Directory& root, Directory& currentDirectory, const string& newDir);
        void rm(const string& filepath);
        void rmdir(const string& filepath);
        void ls(string currentPath, const Directory& dir, const Directory& root) const;
        void lsRecursive(const Directory& dir) const;
        void mkdir(const string& name, const string& path);
        void link(const string& sourceFile, const string& targetName, const Directory& root);
        void cp(const string& sourcePath);

        //Adder/remover functions for the files vector
        void addFile(const shared_ptr<File>& file);
        void removeFile(const vector<shared_ptr<File>>::iterator& it);

        // Reads the contents of the disk
        void readDiskFile();

        // Adds to the contents file if a file is created/updated
        static void addToDiskFile(const FileData& data);

    private:
        vector<shared_ptr<File> > files;

        static void setTimeToNow(FileData& data);

        // This string is marked mutable because the iterator (const function) needs to be able to modify it
        mutable string filesAsString;
        // Receives file information from the files vector and turns it into a string for the iterator
        void updateFilesAsString() const;
    };
} //GTUShell namespace


#endif //DIRECTORY_H
