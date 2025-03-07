#include <iostream>
#include <sstream>
#include <unordered_map>

#include "File.h"
#include "RegularFile.h"
#include "Directory.h"
#include "SoftLinkedFile.h"

using namespace GTUShell;
using namespace std;


enum class Commands {
    ls, mkdir, rm, cp, link, cd, cat, rmdir
};

int main() {
    try {
        // Create an unordered map to check for the command input
        unordered_map<string, Commands> commandMap = {
                {"ls", Commands::ls},
                {"mkdir", Commands::mkdir},
                {"rm", Commands::rm},
                {"cp", Commands::cp},
                {"link", Commands::link},
                {"cd", Commands::cd},
                {"cat", Commands::cat},
                {"rmdir", Commands::rmdir}
        };

        // Set up the data for the root directory
        FileData rootData = {
                'D', ".", "/", "0", 0, ""
        };

        Directory root;
        root.setData(rootData);
        root.readDiskFile();

        Directory currentDirectory = root;
        string currentPath = "/";


        while(true) {
            // Get the command input from the user as a string
            string inputStr;
            inputStr.erase();
            cout << currentPath <<  " > ";
            getline(cin, inputStr);

            // Check if inputStr is empty or it only has whitespaces
            if(inputStr.empty() || inputStr.find_first_not_of(' ') == std::string::npos)
                continue;

            // Parse the words of the string
            istringstream inputss(inputStr);
            string word;
            vector<string> words;
            word.erase();
            while(inputss >> word) {
                words.push_back(word);
            }

            // Make the command word lowercase
            if (!words.empty()) {
                for (auto& c : words[0])
                    c = tolower(c);
            }
            string command = words[0];

            // If command map's end is reached, then the command does not exist in the map
            if(commandMap.find(command) == commandMap.end()) {
                cout << "Command not found: " << command << "\n";
                continue;
            }

            // Execute the commands
            switch (commandMap[command]) {
                case (Commands::ls): {
                    if(words.size() < 2) {
                        currentDirectory.ls(currentPath, currentDirectory, root);
                    } else if(words.size() >= 2 && words[1] == "-R") {
                        currentDirectory.lsRecursive(currentDirectory);
                    }
                    break;
                }
                case (Commands::mkdir): {
                   try {
                       if(words.size() < 2)
                           continue;

                        string dirName = words[1];
                        currentDirectory.mkdir(dirName, currentPath);

                    } catch (DirectoryAlreadyExists& err) {
                        cout << err.what() << "\n";
                    }
                    break;
                }
                case (Commands::rm): {
                    if(words.size() < 2)
                        continue;

                    string fileName = words[1];
                    string pathValue;
                    if(currentPath.back() == '/') pathValue = currentPath + fileName;
                    else pathValue = currentPath + "/" + fileName;

                    try {
                        currentDirectory.rm(pathValue);
                    } catch(PathNotFound& err) {
                        cout << err.what() << "\n";
                    } catch(ContentsFileNotFound& err) {
                        cout << err.what() << "\n";
                    } catch(FileIsDirectory& err) {
                        cout << err.what() << "\n";
                    }

                    break;
                }
                case (Commands::rmdir): {
                    if(words.size() < 2)
                        continue;

                    string fileName = words[1];
                    string pathValue;
                    if(currentPath.back() == '/') pathValue = currentPath + fileName;
                    else pathValue = currentPath + "/" + fileName;

                    try {
                        currentDirectory.rmdir(pathValue);
                    } catch(PathNotFound& err) {
                        cout << err.what() << "\n";
                    } catch(ContentsFileNotFound& err) {
                        cout << err.what() << "\n";
                    } catch(NotDirectory& err) {
                        cout << err.what() << "\n";
                    }
                    break;
                }
                case (Commands::cp): {
                    if(words.size() < 2)
                        continue;
                    string sourcePath = words[1];
                    try {
                        currentDirectory.cp(sourcePath);
                    } catch (PathNotFound& err) {
                        cout << err.what() << "\n";
                    } catch(FileIsDirectory& err) {
                        cout << err.what() << "\n";
                    }


                    break;
                }
                case (Commands::link): {
                    if(words.size() < 3)
                        continue;
                    string sourceFile = words[1];
                    string targetName = words[2];
                    currentDirectory.link(sourceFile, targetName, root);
                    break;
                }
                case (Commands::cd): {
                    if(words.size() < 2)
                        continue;
                    string newDir = words[1];
                    Directory::cd(currentPath, root, currentDirectory, newDir);
                    break;
                }
                case (Commands::cat): {
                    try {
                        //  Flag to see if a command is executed successfully
                        bool executedFlag = false;

                        string filename = words[1];
                        if(!filename.empty()) {
                            for (const auto& filePtr : currentDirectory.getFiles()) {
                                if (filePtr->getName() == filename) {
                                    filePtr->cat();
                                    executedFlag = true;
                                    break;
                                }
                            }
                            if(!executedFlag) {
                                cout << "No such file or directory: " << filename << "\n";
                            }
                        }
                    } catch(const FileNotFound& err) {
                        cout << err.what() << "\n";
                    } catch(const FileIsDirectory& err) {
                        cout << err.what() << "\n";
                    }
                    break;
                }
                default:
                    cout << "Command not found: " << command << "\n";
            }
            // Read the disk file after updating it
            root.readDiskFile();
            try {
                // Check if the disk size exceeds 10MB
                File::checkDiskSize();
            } catch (DiskExceedsLimit& err) {
                cout << err.what() << "\n";
                exit(1);
            }
        }
    } catch(const ContentsFileNotFound& err) {
        // disk.txt was not found, create it
        FileData rootData = {
                'D', ".", "/", "0", 0, ""
        };
        Directory::addToDiskFile(rootData);
        cout << err.what() << "\n";
    } catch(const FileTypeInvalid& err) {
        cout << err.what() << "\n";
    } catch(...) {
        // Catch if some other exception occurs// Iterator overrides
        cout << "Unhandled exception!\n";
        try {
            throw; // Throw the exception again to get the details
        } catch (const exception& err) {
            cout << "Details: " << err.what() << "\n";
        } catch (...) {
            cout << "Unable to get more information." << "\n";
        }
    }
    return 0;
}