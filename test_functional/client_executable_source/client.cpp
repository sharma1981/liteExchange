#include "client_request.h"
#include "client_application.h"

#include <string>
#include <exception>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <boost/format.hpp>

#include <core/file_utility.h>
#include <core/string_utility.h>
#include <core/datetime_utility.h>
#include <core/pretty_exception.h>
#include <core/self_process.h>

namespace program_errors
{
    const int INVALID_CONFIG_FILE = 1;
    const int ALREADY_RUNNING = 2;
    const int RUNTIME_ERROR = 3;
    const int INSUFFICIENT_MEMORY = 4;
    const int UNKNOWN_PROBLEM = 5;
}

void onError(const string& message, int exit_code);

void createQuickFixConfigFile(const string& templateFile, const string& server, const string &clientName, const string& outputFileName);

int main(int argc, char** argv)
{
    if (argc != 5)
    {
        std::cout << "usage: " << argv[0]
            << " quickfix_config_template_file server_address client_name testcase_file" << std::endl;
        return 0;
    }

    try
    {
        string quickFixTemplateFile = argv[1];
        string targetServer = argv[2];
        string clientName = argv[3];
        string csvTestFile = argv[4];

        // Set current working directory as current executable`s directory
        core::SelfProcess::setCurrentWorkingDirectory(core::SelfProcess::getCurrentExecutableDirectory());

        string quickFixConfigFile = clientName + ".cfg";
        createQuickFixConfigFile(quickFixTemplateFile, targetServer, clientName, quickFixConfigFile);

        if (quickFixConfigFile.length() < 5) { throw std::invalid_argument("Invalid FIX engine config file"); }

        // Run the application
        ClientApplication application(csvTestFile, quickFixConfigFile);
        application.run();

        core::deleteFile(quickFixConfigFile);
    }
    catch (std::invalid_argument & e)
    {
        onError(e.what(), program_errors::RUNTIME_ERROR);
    }
    catch (std::runtime_error & e)
    {
        onError(e.what(), program_errors::RUNTIME_ERROR);
    }
    catch (std::bad_alloc& e)
    {
        onError(e.what(), program_errors::INSUFFICIENT_MEMORY);
    }
    catch (...)
    {
        onError("Unknown exception caught", program_errors::UNKNOWN_PROBLEM);
    }

    return 0;
}

void onError(const string& message, int exit_code)
{
    std::cerr << message << std::endl;
    std::exit(exit_code);
}

void createQuickFixConfigFile(const string& templateFile, const string& server, const string &clientName, const string &outputFileName)
{
    // Delete out file if already exists
    if( core::doesFileExist(outputFileName))
    {
        core::deleteFile(outputFileName);
    }
    // Get template file contents as string
    ifstream templateFileStream(templateFile);
    string templateFileData( (istreambuf_iterator<char>(templateFileStream)), istreambuf_iterator<char>());
    // Inject server and client name into it
    core::replaceInString(templateFileData, "%CLIENT%", clientName);
    core::replaceInString(templateFileData, "%SERVER%", server);
    // Output
    ofstream outFile;
    outFile.open (outputFileName);
    outFile << templateFileData;
    outFile.close();
}