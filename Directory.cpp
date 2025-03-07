
#include "Directory.h"
#include "RegularFile.h"
#include "SoftLinkedFile.h"
using namespace std;

namespace GTUShell {

    File::iterator Directory::begin() const {
        // In order to return the file information as std::string::const_iterator, this function
        // receives file information from the files vector and turns it into a string
        updateFilesAsString();
        // Return the newly updated string as the beginning iterator
        return filesAsString.begin();
    }

    File::iterator Directory::end() const {
        // In order to return the file information as std::string::const_iterator, this function
        // receives file information from the files vector and turns it into a string
        updateFilesAsString();
        // Return the newly updated string as the end iterator
        return filesAsString.end();
    }

    void Directory::readDiskFile() {
        // Clear the previously saved files
        files.clear();

        // Try to open the disk
        ifstream inputStream("disk.txt");
        if (!inputStream.is_open())
            throw ContentsFileNotFound();

        string line;
        string placeholder = "~0~";

        // Store directories by their paths
        map<string, shared_ptr<Directory>> directories;

        while (getline(inputStream, line)) {
            // Turn the line into a string stream so that getline function can work on it
            stringstream ss(line);

            // This FileData object will hold the information of the file that is currently being read
            FileData temp;
            string tempType;
            getline(ss, tempType, '\t');

            // Check the type to see if it is a type the system knows
            if (tempType != "F" && tempType != "S" && tempType != "D") {
                throw FileTypeInvalid();

            }

            temp.type = tempType[0]; // Get the file type character
            getline(ss, temp.path, '\t'); // Save the path as a string
            getline(ss, temp.name, '\t'); // Save the name as a string
            getline(ss, temp.date, '\t'); // Save the date as a string
            ss >> temp.size; // Save the size as an integer

            // Read the next line (placeholder\ncontent\nplaceholder is the format)
            bool readingContent = false;
            while (getline(inputStream, line)) {
                if (line == placeholder) {
                    if (readingContent) {
                        // First placeholder is passed, continue to read the content
                        break;
                    } else {
                        // Currently at the first placeholder, start reading after it
                        readingContent = true;
                        continue;
                    }
                }

                if (readingContent) {
                    if (!temp.content.empty()) {
                        temp.content += '\n'; // Add a new line if not at the first line
                    }

                    temp.content += line;
                }
            }

            // Make a File shared_ptr to be stored in the files vector with the proper File type
            shared_ptr<File> filePtr;

            switch (temp.type) {
                case 'F': // Regular File
                    filePtr = std::make_shared<RegularFile>(temp);
                    break;
                case 'S': // Soft Linked File
                    filePtr = std::make_shared<SoftLinkedFile>(temp, make_shared<Directory>(*this));
                    break;
                case 'D': // Directory
                    filePtr = std::make_shared<Directory>();
                    break;
                default:
                    throw FileTypeInvalid();
            }

            filePtr->setData(temp);

            // Make the hierarchy of nested files by creating parent directories/inserting their files

            string parentPath;
            // Save the directories into the map
            directories[temp.path] = std::static_pointer_cast<Directory>(filePtr);

            // Find the position of the last slash for finding the parent directory
            size_t lastSlash = temp.path.find_last_of("/");
            if (lastSlash != string::npos) {
                // If slash was found
                // Create the parent path by creating a substr
                parentPath = temp.path.substr(0, lastSlash);
                if (parentPath.empty()) parentPath = "/"; // Root directory
                if (directories.find(parentPath) != directories.end()) {
                    // Parent directory exists, add the file in it
                    directories[parentPath]->addFile(filePtr);
                } else {
                    // Parent directory does not exist, create it
                    shared_ptr<Directory> parentDir = std::make_shared<Directory>();
                    directories[parentPath] = parentDir;
                    directories[parentPath]->addFile(filePtr);
                }
            }

            // Only add the file to the files vector if the path is root
            if (parentPath == "/") {
                files.push_back(filePtr);
            }
        }

        inputStream.close();
    }

    void Directory::setTimeToNow(FileData& data) {
        auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm localTime = *std::localtime(&currentTime);

        char formattedTime[50];
        std::strftime(formattedTime, sizeof(formattedTime), "%b %d %Y %H:%M", &localTime);

        data.date = formattedTime;
    }

    void Directory::addToDiskFile(const FileData& data) {
        std::ofstream outputStream("disk.txt", std::ios_base::app);

        if (!outputStream.is_open())
            throw ContentsFileNotFound();

        auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm localTime = *std::localtime(&currentTime);

        char formattedTime[50];
        std::strftime(formattedTime, sizeof(formattedTime), "%b %d %Y %H:%M", &localTime);

        outputStream << data.type << "\t" << data.path << "\t" << data.name << "\t" << formattedTime << "\t" << data.size
                     << "\n" << "~0~" << "\n" << data.content << "\n" << "~0~" << "\n";

        outputStream.close();
    }

    void Directory::updateFilesAsString() const {
        // Turns the files vector's information into a string for the iterator to use
        filesAsString.clear();
        string typeVal;

        // Add the type and name of the file into the string
        for (const auto& filePtr : files) {
            typeVal = string(1, filePtr->getType());
            filesAsString += typeVal + "\t" + filePtr->getName() + "\n";
        }

        // Remove the last \n
        if(!filesAsString.empty())
            filesAsString.erase(filesAsString.size() - 1);
    }

    vector<shared_ptr<File> > Directory::getFiles() const {
        return files;
    }

    void Directory::addFile(const shared_ptr<File>& file) {
        files.push_back(file);
    }

    void Directory::removeFile(const vector<shared_ptr<File>>::iterator& it) {
        files.erase(it);
    }

    void Directory::ls(string currentPath, const Directory& dir, const Directory& root) const {
        // If not in root (.) directory:
        // Print the current directory information with the name "."
        // ---
        // Find the parent directory and print it with the name ".."
        // ---
        // Print all the files inside the current directory

        char type = this->getType();
        string name = this->getName();
        string date = this->getDate();
        string path;

            if (name != ".") {
                cout << std::left << std::setw(4) << type << std::setw(20) << "."
                     << date << "\n";

                // ---

                // Find the parent directory
                size_t lastSlash = currentPath.find_last_of("/");
                currentPath = (lastSlash == 0) ? "/" : currentPath.substr(0, lastSlash);

                bool success = false;
                // Now currentPath holds the path of the parentDirectory, find it and display it's information
                for (const auto &filePtr: root.getFiles()) {
                    type = filePtr->getType();
                    date = filePtr->getDate();
                    path = filePtr->getPath();

                    if (type == 'D' && path == currentPath) {
                        cout << std::left << std::setw(4) << type << std::setw(20) << ".."
                             << date << "\n";
                        success = true;
                        break;
                    }
                }
                if (!success) {
                    cout << std::left << std::setw(4) << 'D' << std::setw(20) << ".."
                         << date << "\n";
                }
            }

        // ---

        int size;
        // Print the files inside the directory
        for (const auto &filePtr: dir.getFiles()) {
            type = filePtr->getType();
            name = filePtr->getName();
            date = filePtr->getDate();
            size = filePtr->getSize();

            if (type == 'F') {
                // Type is RegularFile will print the size of it
                cout << std::left << std::setw(4) << type << std::setw(20) << name
                     << date << "\t" << size << "\n";
            } else {
                // Type is DirectorySoftLinkedFile, will not print the size of it
                cout << std::left << std::setw(4) << type << std::setw(20) << name
                     << date << "\n";
            }
        }
    }

    void Directory::lsRecursive(const GTUShell::Directory &dir) const {
        // ls -R
        char type;
        string path;
        string name;
        for(const auto& filePtr : dir.getFiles()) {
            type = filePtr->getType();
            path = filePtr->getPath();
            name = filePtr->getName();
            if(type == 'D' && path != "/") {
                cout << std::left << std::setw(4) << type << std::setw(20) << name << "\t" << path << "\n";
                auto subDir = std::dynamic_pointer_cast<Directory>(filePtr);
                if (subDir && subDir.get() != this)  // Avoid going through the same directory again
                    lsRecursive(*subDir); // Recursively go through the subdirectory
            } else {
                cout << std::left << std::setw(4) << type << std::setw(20) << name << "\t" << path << "\n";
            }
        }
    }

    void Directory::mkdir(const string& dirName, const string& currentPath) {
        // If the directory with the same name already exists, throw an exception.
        for(const auto& filePtr : getFiles()) {
            if(filePtr -> getType() == 'D' && filePtr -> getName() == dirName) {
                throw DirectoryAlreadyExists(dirName);
            }
        }

        // Add a / to the path if needed
        string pathValue;
        if(currentPath.back() == '/') pathValue = currentPath + dirName;
        else pathValue = currentPath + "/" + dirName;

        // Create the data of the file
        FileData dirData = { 'D', dirName, pathValue, "0", 0, "" };
        setTimeToNow(dirData);

        // Create the directory with the specified data and add it to both disk and memory
        Directory newDir;
        newDir.setData(dirData);

        addFile(std::make_shared<Directory>(newDir));
        addToDiskFile(dirData);
    }

    void Directory::cd(string& currentPath, const Directory& root, Directory& currentDirectory, const string& newDir) {

        if(newDir.empty() || newDir == ".") {
            // Do nothing
        } else if (newDir == ".." && currentPath != "/") {
            // Change the directory one up, to the parent (if currently not in root)

            // Find the parent directory
            size_t lastSlash = currentPath.find_last_of("/");
            currentPath = (lastSlash == 0) ? "/" : currentPath.substr(0, lastSlash);
            currentDirectory = root;

            istringstream pathStream(currentPath);
            string dirName;
            while (getline(pathStream, dirName, '/')) {
                for (const auto& filePtr : currentDirectory.getFiles()) {
                    if (filePtr->getType() == 'D' && filePtr->getName() == dirName) {
                        // Change the currentDir to the parent directory that was found
                        currentDirectory = *(dynamic_pointer_cast<Directory>(filePtr));
                        break;
                    }
                }
            }
        } else { // Not a special input
            // Change the directory to the new one
            bool executedFlag = false;
            for (const auto& filePtr : currentDirectory.getFiles()) {
                if (filePtr->getType() == 'D' && filePtr->getName() == newDir) {
                    currentDirectory = *(dynamic_pointer_cast<Directory>(filePtr));
                    currentPath = filePtr->getPath();
                    executedFlag = true;
                    break;
                }
            }
            if (!executedFlag) {
                cout << "No such directory: " << newDir << "\n";
            }
        }
    }

    void Directory::link(const string& sourceFile, const string& targetName, const Directory& root) {
        // Creates a new file named targetFile which will have the path of sourceFile in its content
        string sourceFilePath;
        for(const auto& filePtr : files) {
            if(filePtr->getType() == 'F' && filePtr->getName() == sourceFile) {
                sourceFilePath = filePtr->getPath();
                break;
            }
        }

        if(sourceFilePath.empty()) {
            cout << "No such file: " << targetName << "\n";
            return;
        }

        string currentPath = getPath();
        string pathValue;

        // Add / if needed
        if(currentPath.back() == '/') pathValue = currentPath + targetName;
        else pathValue = currentPath + "/" + targetName;

        // Specify the information of the new file
        FileData temp = {
                'S', targetName, pathValue, "0", 0, sourceFilePath
        };

        setTimeToNow(temp);
        addToDiskFile(temp);
        addFile(std::make_shared<SoftLinkedFile>(temp, make_shared<Directory>(root)));
    }

    void Directory::rm(const string &fileToRmPath) {
        ifstream inputFile("disk.txt");
        ofstream tempFile("temp.txt");

        bool fileFound = false;
        bool inContentBlock = false;
        int skipCounter = 0;
        string placeholder = "~0~";

        if (!inputFile.is_open() || !tempFile.is_open()) {
            throw ContentsFileNotFound();
        }

        // Read the content until reaching the placeholder for the second time
        string line;
        while (getline(inputFile, line)) {
            if (inContentBlock) {
                // Check for the end of content block
                if (line == placeholder && skipCounter != 0) {
                    inContentBlock = false;
                }
                skipCounter++;
                continue;
            }

            // Create an input string stream to read the type and path values of each line with file information
            istringstream iss(line);
            string type, filePath;
            iss >> type >> filePath;

            if (filePath == fileToRmPath) {
                if (type == "D") {
                    throw FileIsDirectory(fileToRmPath);
                } else {
                    // Mark file found and skip writing both lines for the file
                    fileFound = true;
                    inContentBlock = true;
                    continue;
                }
            }

            // Write every file except the one to be removed
            tempFile << line << '\n';
        }

        inputFile.close();
        tempFile.close();

        // Make the temp file the new disk file
        remove("disk.txt");
        rename("temp.txt", "disk.txt");

        if(fileFound) {
            // Remove the file from the files vector of the currentDirectory
            for(auto it = files.begin() ; it != files.end() ; it++) {
                if((*it)->getType() != 'D' && ((*it)->getPath() == fileToRmPath)) {
                    removeFile(it);
                    break;
                }
            }
        } else {
            // File was not found inside the disk
            throw PathNotFound(fileToRmPath);
        }
    }

    void Directory::rmdir(const string &fileToRmPath) {
        ifstream inputFile("disk.txt");
        ofstream tempFile("temp.txt");

        bool fileFound = false;
        bool inContentBlock = false;
        int skipCounter = 0;
        string placeholder = "~0~";

        if (!inputFile.is_open() || !tempFile.is_open()) {
            throw ContentsFileNotFound();
        }

        string line;
        while (getline(inputFile, line)) {
            if (inContentBlock) {
                // Check for the end of content block
                if (line == placeholder && skipCounter != 0) {
                    inContentBlock = false;
                }
                skipCounter++;
                continue;
            }

            // Create an input string stream to read the type and path values of each line with file information
            istringstream iss(line);
            string type, filePath;
            iss >> type >> filePath;

            if (filePath == fileToRmPath) {
                if (type == "D") {
                    // Mark file found and skip writing both lines for the file
                    fileFound = true;
                    inContentBlock = true;
                    continue;
                } else {
                    throw NotDirectory(fileToRmPath);
                }
            }

            // Write every file except the one to be removed
            tempFile << line << '\n';
        }

        inputFile.close();
        tempFile.close();

        // Make the temp file the new disk file
        remove("disk.txt");
        rename("temp.txt", "disk.txt");

        if(fileFound) {
            // Remove the file from the files vector of the currentDirectory
            for(auto it = files.begin() ; it != files.end() ; it++) {
                if((*it)->getType() == 'D' && ((*it)->getPath() == fileToRmPath)) {
                    removeFile(it);
                    break;
                }
            }
        } else {
            // File was not found inside the disk
            throw PathNotFound(fileToRmPath);
        }
    }

    void Directory::cp(const string& path) {
        // Try to open the file from the regular OS and copy file from there
        ifstream inputStream(path);
        if(inputStream.is_open()) {
            FileData temp;
            size_t lastSlash = path.find_last_of("/");

            if (lastSlash != string::npos) {
                // Set the file name, do not include the last slash
                temp.name = path.substr(lastSlash+1, path.size());
            }
            string line;
            while(getline(inputStream, line)) {
                // Save that file's content
                temp.content += (line + "\n");
            }
            // Remove the last \n
            temp.content.erase(temp.content.size()-1);
            temp.type = 'F';

            string pathValue;
            string currentPath = getPath();
            // Add a / if needed
            if(currentPath.back() == '/') pathValue = currentPath + temp.name;
            else pathValue = currentPath + "/" + temp.name;
            temp.path = pathValue;
            temp.size = (temp.content).size();

            // Add the newly copied file into the disk and memory
            setTimeToNow(temp);
            addToDiskFile(temp);
            auto filePtr = make_shared<RegularFile>(temp);
            addFile(filePtr);
        } else {
            // Look for the file in the currentDirectory
            string newName;
            string newContent;
            string newPath;
            char newType;
            int newSize;
            bool executedFlag = false;
            for(const auto& filePtr : getFiles()) {
                if(filePtr->getName() == path) {
                    newName = filePtr->getName();
                    newPath = filePtr->getPath();
                    newContent = filePtr->getContent();
                    newType = filePtr->getType();
                    newSize = filePtr->getSize();
                    executedFlag = true;
                    break;
                }
            }
            if(executedFlag) {
                if(newType == 'D')
                    throw FileIsDirectory(newName);

                FileData newFile;
                newFile.name = "copy_" + newName;
                newFile.content = newContent;
                newFile.type = 'F';
                newFile.size = newSize;
                newFile.path = newPath;

                string currentPath = getPath();
                if(currentPath.back() == '/') newFile.path = currentPath + newFile.name;
                else newFile.path = currentPath + "/" + newFile.name;

                newFile.size = (newFile.content).size();
                setTimeToNow(newFile);

                // Add the new file into the disk and the memory
                addToDiskFile(newFile);
                shared_ptr<File> sharedFilePtr = make_shared<RegularFile>(newFile);
                addFile(sharedFilePtr);
            } else {
                throw PathNotFound(path);
            }
        }

    }

}