//
// Created by amirhossein on 1/6/2025.
//
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream> // For file operations

// Function to compile a C++ source file
bool compile(const std::string &filename)
{
    std::string compileCommand = "g++ " + filename + " -o output"; // Adjust as needed
    int result = std::system(compileCommand.c_str());
    return (result == 0);
}

// Function to run an executable
void run(const std::string &filename)
{
    std::string runCommand = ".\\" + filename + ".exe ";
    std::system(runCommand.c_str());
}

void run_in_another_window(const std::string &filename)
{
#ifdef _WIN32 // Windows
    std::string runCommand = "start cmd /c \"" + filename + " & pause\"";
#elif __linux__ // Linux
    std::string runCommand = "x-terminal-emulator -e \"" + filename + "; read -n 1 -s -r -p 'Press any key to continue...'\"";
#elif __APPLE__ // macOS
    std::string runCommand = "osascript -e 'tell application \"Terminal\" to do script \"" + filename + "\"'";
#else
    std::cerr << "Unsupported operating system." << std::endl;
    return;
#endif

    std::system(runCommand.c_str());
}

// Function to "debug" (simulated basic debugging)
void debug(const std::string &filename)
{
    std::cout << "Debugging " << filename << " (simulated):\n";
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string line;
        int lineNumber = 1;
        while (std::getline(file, line))
        {
            std::cout << lineNumber << ": " << line << std::endl;
            lineNumber++;
        }
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file for debugging." << std::endl;
    }
}

int main()
{
    std::string filename;
    std::string command;

    while (true)
    {
        std::cout << "IDE> ";
        std::cin >> command;

        if (command == "compile")
        {
            std::cout << "Enter filename to compile: ";
            std::cin >> filename;
            if (compile(filename))
            {
                std::cout << "Compilation successful!\n";
            }
            else
            {
                std::cout << "Compilation failed!\n";
            }
        }
        else if (command == "run")
        {
            run("output"); // Assuming the output executable is named "output"
        }
        else if (command == "runw")
        {
            run_in_another_window("output"); // Assuming the output executable is named "output"
        }
        else if (command == "debug")
        {
            std::cout << "Enter filename to debug: ";
            std::cin >> filename;
            debug(filename);
        }
        else if (command == "exit")
        {
            break;
        }
        else
        {
            std::cout << "Invalid command.\n";
        }
    }

    return 0;
}