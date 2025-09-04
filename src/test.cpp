//
// libdof test app
//

#include "DOF/DOF.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#endif

const int TIMEOUT_OFF = 650;
const int TIMEOUT_ON = 650;
const int TIMEOUT_START_DELAY = 1000;

struct TestRom
{
   std::string name;
   std::string description;
};

std::vector<TestRom> testRoms = { { "ij_l7", "Indiana Jones L7" }, { "tna", "Total Nuclear Annihilation" }, { "gw", "The Getaway High Speed II" }, { "goldcue", "Gold Cue" },
   { "bourne", "Bourne Identity" }, { "twenty4", "24" } };

void LIBDOFCALLBACK LogCallback(DOF_LogLevel logLevel, const char* format, va_list args)
{
   const char* levelStr;
   switch (logLevel)
   {
   case DOF_LogLevel_WARN: levelStr = "[WARN] "; break;
   case DOF_LogLevel_ERROR: levelStr = "[ERROR]"; break;
   case DOF_LogLevel_DEBUG: levelStr = "[DEBUG]"; break;
   case DOF_LogLevel_INFO: levelStr = "[INFO] "; break;
   default: break;
   }

   va_list args_copy;
   va_copy(args_copy, args);
   int size = vsnprintf(nullptr, 0, format, args_copy);
   va_end(args_copy);

   if (size > 0)
   {
      char* const buffer = static_cast<char*>(malloc(size + 1));
      vsnprintf(buffer, size + 1, format, args);
      printf("%s %s\n", levelStr, buffer);
      free(buffer);
   }
}

void Log(const char* format, ...)
{
   va_list args;
   va_start(args, format);
   LogCallback(DOF_LogLevel_INFO, format, args);
   va_end(args);
}

void TriggerOutput(DOF::DOF* pDof, char type, int id, int value, int wait_ms = 0)
{
   std::stringstream ss;
   ss << "=== Triggering " << type << id << ": " << (value ? "on" : "off") << " ===";
   Log(ss.str().c_str());
   pDof->DataReceive(type, id, value);
   if (wait_ms > 0)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
   }
}

void TriggerOutputOnOff(DOF::DOF* pDof, char type, int id, int on_duration_ms = TIMEOUT_ON, int off_duration_ms = TIMEOUT_OFF)
{
   std::stringstream ss;
   ss << "=== Triggering " << type << id << ": on for ";

   if (on_duration_ms > 999)
   {
      ss << std::fixed << std::setprecision(2) << on_duration_ms / 1000.0 << "s";
   }
   else
   {
      ss << on_duration_ms << "ms";
   }

   ss << ", off for ";

   if (off_duration_ms > 999)
   {
      ss << std::fixed << std::setprecision(2) << off_duration_ms / 1000.0 << "s";
   }
   else
   {
      ss << off_duration_ms << "ms";
   }

   ss << " ===";

   Log(ss.str().c_str());

   pDof->DataReceive(type, id, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(on_duration_ms));
   pDof->DataReceive(type, id, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(off_duration_ms));
}

void RunIJTests(DOF::DOF* pDof)
{
   Log("========================================");
   Log("Testing ROM: ij_l7");
   Log("========================================");
   pDof->Init("", "ij_l7");

   std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT_START_DELAY));

   TriggerOutputOnOff(pDof, 'S', 9);
   TriggerOutputOnOff(pDof, 'S', 12);
   TriggerOutputOnOff(pDof, 'S', 51);
   TriggerOutputOnOff(pDof, 'S', 53);
   TriggerOutputOnOff(pDof, 'W', 15);
   TriggerOutputOnOff(pDof, 'W', 16);
   TriggerOutputOnOff(pDof, 'W', 65);
   TriggerOutputOnOff(pDof, 'W', 66);
   TriggerOutputOnOff(pDof, 'W', 67);
   TriggerOutputOnOff(pDof, 'W', 68);
   TriggerOutputOnOff(pDof, 'L', 88);
   TriggerOutputOnOff(pDof, 'S', 10);
   TriggerOutputOnOff(pDof, 'W', 88);

   pDof->Finish();
}

void RunTNATests(DOF::DOF* pDof)
{
   pDof->Init("", "tna");

   Log("========================================");
   Log("Testing ROM: tna");
   Log("========================================");
   ;

   std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT_START_DELAY));

   TriggerOutputOnOff(pDof, 'E', 103);
   TriggerOutputOnOff(pDof, 'E', 108);
   TriggerOutputOnOff(pDof, 'E', 110);
   TriggerOutputOnOff(pDof, 'E', 112);
   TriggerOutputOnOff(pDof, 'E', 116);
   TriggerOutputOnOff(pDof, 'E', 144);
   TriggerOutputOnOff(pDof, 'E', 146);
   TriggerOutputOnOff(pDof, 'E', 147);
   TriggerOutputOnOff(pDof, 'E', 148);
   TriggerOutputOnOff(pDof, 'E', 149);
   TriggerOutputOnOff(pDof, 'E', 150);
   TriggerOutputOnOff(pDof, 'E', 151);
   TriggerOutputOnOff(pDof, 'E', 152);
   TriggerOutputOnOff(pDof, 'E', 153);
   TriggerOutputOnOff(pDof, 'E', 179);

   pDof->Finish();
}

void RunGWTests(DOF::DOF* pDof)
{
   pDof->Init("", "gw");

   Log("========================================");
   Log("Testing ROM: gw");
   Log("========================================");

   std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT_START_DELAY));

   Log("=== DOF Protocol Test Scenarios ===");

   // S46 & S48 also require W78 to be on

   TriggerOutputOnOff(pDof, 'L', 52);
   TriggerOutputOnOff(pDof, 'S', 8);
   TriggerOutputOnOff(pDof, 'S', 12);
   TriggerOutputOnOff(pDof, 'S', 16);
   TriggerOutputOnOff(pDof, 'S', 19);
   TriggerOutputOnOff(pDof, 'S', 46);
   TriggerOutputOnOff(pDof, 'S', 48);
   TriggerOutputOnOff(pDof, 'W', 15);
   TriggerOutputOnOff(pDof, 'W', 25);
   TriggerOutputOnOff(pDof, 'W', 26);
   TriggerOutputOnOff(pDof, 'W', 37);
   TriggerOutputOnOff(pDof, 'W', 38);
   TriggerOutputOnOff(pDof, 'W', 42);
   TriggerOutputOnOff(pDof, 'W', 43);
   TriggerOutputOnOff(pDof, 'W', 52);
   TriggerOutputOnOff(pDof, 'W', 53);
   TriggerOutputOnOff(pDof, 'W', 67);
   TriggerOutputOnOff(pDof, 'W', 78);
   TriggerOutputOnOff(pDof, 'W', 81);
   TriggerOutputOnOff(pDof, 'W', 86);
   TriggerOutputOnOff(pDof, 'W', 87);
   TriggerOutputOnOff(pDof, 'W', 88);

   pDof->Finish();
}

void RunGoldcueTests(DOF::DOF* pDof)
{
   pDof->Init("", "goldcue");

   Log("========================================");
   Log("Testing ROM: goldcue");
   Log("========================================");

   std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT_START_DELAY));

   TriggerOutputOnOff(pDof, 'L', 73);
   TriggerOutputOnOff(pDof, 'L', 74);
   TriggerOutputOnOff(pDof, 'L', 75);
   TriggerOutputOnOff(pDof, 'L', 76);
   TriggerOutputOnOff(pDof, 'L', 77);
   TriggerOutputOnOff(pDof, 'L', 78);
   TriggerOutputOnOff(pDof, 'S', 3);
   TriggerOutputOnOff(pDof, 'S', 17);

   pDof->Finish();
}

void RunBourneTests(DOF::DOF* pDof)
{
   pDof->Init("", "bourne");

   Log("========================================");
   Log("Testing ROM: bourne");
   Log("========================================");

   std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT_START_DELAY));

   TriggerOutputOnOff(pDof, 'E', 142, 1000);
   TriggerOutputOnOff(pDof, 'E', 145, 1000);
   TriggerOutputOnOff(pDof, 'E', 103);
   TriggerOutputOnOff(pDof, 'E', 111, 5000);
   TriggerOutputOnOff(pDof, 'E', 181, 1000);
   TriggerOutputOnOff(pDof, 'E', 199, 1000);

   pDof->Finish();
}

void RunTwenty4Tests(DOF::DOF* pDof)
{
   pDof->Init("", "twenty4");

   Log("========================================");
   Log("Testing ROM: twenty4");
   Log("========================================");

   std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT_START_DELAY));

   TriggerOutputOnOff(pDof, 'S', 9);
   TriggerOutputOnOff(pDof, 'S', 19);
   TriggerOutputOnOff(pDof, 'S', 31);
   TriggerOutputOnOff(pDof, 'S', 32);
   TriggerOutputOnOff(pDof, 'W', 3);
   TriggerOutputOnOff(pDof, 'W', 10);
   TriggerOutputOnOff(pDof, 'W', 46);

   TriggerOutputOnOff(pDof, 'S', 17);
   TriggerOutputOnOff(pDof, 'S', 26);

   TriggerOutputOnOff(pDof, 'S', 11);
   TriggerOutputOnOff(pDof, 'W', 43);
   TriggerOutputOnOff(pDof, 'W', 55);

   TriggerOutputOnOff(pDof, 'S', 18);
   TriggerOutputOnOff(pDof, 'S', 26);
   TriggerOutputOnOff(pDof, 'W', 62);
   TriggerOutputOnOff(pDof, 'W', 56);

   TriggerOutputOnOff(pDof, 'S', 10);
   TriggerOutputOnOff(pDof, 'S', 20);
   TriggerOutputOnOff(pDof, 'S', 27);
   TriggerOutputOnOff(pDof, 'S', 31);
   TriggerOutputOnOff(pDof, 'W', 54);
   TriggerOutputOnOff(pDof, 'W', 57);

   TriggerOutputOnOff(pDof, 'S', 8, 1500);
   TriggerOutputOnOff(pDof, 'W', 62, 1500);

   TriggerOutputOnOff(pDof, 'S', 24, 200, 800);

   pDof->Finish();
}

std::string GetDefaultBasePath()
{
#ifdef _WIN32
   char cwd[1024];
   if (getcwd(cwd, sizeof(cwd)) != nullptr)
   {
      return std::string(cwd) + "\\";
   }
   return ".\\\\";
#else
   const char* homeDir = getenv("HOME");
   if (homeDir == nullptr)
   {
      struct passwd* pwd = getpwuid(getuid());
      if (pwd)
         homeDir = pwd->pw_dir;
   }

   if (homeDir != nullptr)
   {
      return std::string(homeDir) + "/.vpinball/";
   }
   return "./";
#endif
}

void PrintUsage(const char* programName)
{
   std::cout << "Usage: " << programName << " [ROM_NAME] [--base-path PATH]" << std::endl;
   std::cout << std::endl;
   std::cout << "Options:" << std::endl;
   std::cout << "  ROM_NAME     Specific ROM to test (optional)" << std::endl;
   std::cout << "  --base-path  Custom base path for DOF configuration" << std::endl;
   std::cout << "               Default: Windows = current directory" << std::endl;
   std::cout << "                       Linux/Mac = ~/.vpinball/" << std::endl;
   std::cout << std::endl;
   std::cout << "Available ROMs:" << std::endl;
   for (const auto& testRom : testRoms)
   {
      std::cout << "  " << std::left << std::setw(12) << testRom.name << " " << testRom.description << std::endl;
   }
}

int main(int argc, const char* argv[])
{
   std::string basePath = GetDefaultBasePath();
   std::string romName = "";

   for (int i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "--base-path") == 0 && i + 1 < argc)
      {
         basePath = argv[i + 1];
         if (basePath.back() != '/' && basePath.back() != '\\')
         {
#ifdef _WIN32
            basePath += "\\";
#else
            basePath += "/";
#endif
         }
         i++;
      }
      else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
      {
         PrintUsage(argv[0]);
         return 0;
      }
      else if (argv[i][0] != '-')
      {
         romName = argv[i];
      }
   }

   DOF::Config* pConfig = DOF::Config::GetInstance();
   pConfig->SetLogCallback(LogCallback);
   pConfig->SetLogLevel(DOF_LogLevel_DEBUG);
   pConfig->SetBasePath(basePath.c_str());

   Log("Using base path: %s", basePath.c_str());

   DOF::DOF* pDof = new DOF::DOF();

   if (!romName.empty())
   {
      bool found = false;
      for (const auto& testRom : testRoms)
      {
         if (testRom.name == romName)
         {
            found = true;
            if (testRom.name == "ij_l7")
               RunIJTests(pDof);
            else if (testRom.name == "tna")
               RunTNATests(pDof);
            else if (testRom.name == "gw")
               RunGWTests(pDof);
            else if (testRom.name == "goldcue")
               RunGoldcueTests(pDof);
            else if (testRom.name == "bourne")
               RunBourneTests(pDof);
            else if (testRom.name == "twenty4")
               RunTwenty4Tests(pDof);
            break;
         }
      }

      if (!found)
      {
         std::cout << "Unknown ROM: " << romName << std::endl;
         std::cout << "Available ROMs: ";
         for (size_t i = 0; i < testRoms.size(); i++)
         {
            std::cout << testRoms[i].name;
            if (i < testRoms.size() - 1)
               std::cout << ", ";
         }
         std::cout << std::endl;
         delete pDof;
         return 1;
      }
   }
   else
   {
      RunIJTests(pDof);
      RunTNATests(pDof);
      RunGWTests(pDof);
      RunGoldcueTests(pDof);
      RunBourneTests(pDof);
      RunTwenty4Tests(pDof);
   }

   Log("Shutting down...");
   delete pDof;

   Log("All tests completed!");

   return 0;
}
