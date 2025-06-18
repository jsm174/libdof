#include "DirectOutputHandler.h"
#include "Log.h"
#include "general/StringExtensions.h"

#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <climits>
#elif defined(__linux__)
#include <unistd.h>
#include <climits>
#endif

namespace DOF
{

std::string DirectOutputHandler::GetInstallFolder()
{
   // Get the full path to the running executable
   std::string executablePath;

#ifdef _WIN32
   char buffer[MAX_PATH];
   DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
   if (length > 0 && length < MAX_PATH)
   {
      executablePath = std::string(buffer, length);
   }
#elif defined(__APPLE__)
   char buffer[PATH_MAX];
   uint32_t size = PATH_MAX;
   if (_NSGetExecutablePath(buffer, &size) == 0)
   {
      executablePath = std::string(buffer);
   }
#elif defined(__linux__)
   char buffer[PATH_MAX];
   ssize_t length = readlink("/proc/self/exe", buffer, PATH_MAX - 1);
   if (length != -1)
   {
      buffer[length] = '\0';
      executablePath = std::string(buffer);
   }
#endif

   if (executablePath.empty())
      return "";

   // Get the directory path of the executable
   std::filesystem::path execPath(executablePath);
   std::string assemblyPath = execPath.parent_path().string();

   // Check for the existence of a Config folder in this directory
   std::filesystem::path assemblyConfigPath = std::filesystem::path(assemblyPath) / "Config";
   std::filesystem::path assemblyParentConfigPath = std::filesystem::path(assemblyPath).parent_path() / "Config";

   if (!std::filesystem::exists(assemblyConfigPath) && std::filesystem::exists(assemblyParentConfigPath))
   {
      // New configuration with binary subfolders - the executable is in
      // a subfolder within the install folder, so the install folder
      // is the parent of the executable folder
      std::string parent = std::filesystem::path(assemblyPath).parent_path().string();
      Log::Once("InstallFolderLoc",
         StringExtensions::Build("Install folder lookup: executable: {0}, install folder: {1} (PARENT of the executable folder -> new shared x86/x64 install)", executablePath, parent));
      return parent;
   }
   else
   {
      // Old flat configuration - the executable is in the install folder
      Log::Once("InstallFolderLoc",
         StringExtensions::Build("Install folder lookup: executable: {0}, install folder: {1} (EXECUTABLE folder -> original flat install configuration)", executablePath, assemblyPath));
      return assemblyPath;
   }
}

DirectOutputHandler::DirectOutputHandler() { }
DirectOutputHandler::~DirectOutputHandler() { }

}