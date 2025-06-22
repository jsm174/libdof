//
// libdof test app
//
// The following entries should be in
// ~/.vpinball/directoutputconfig/directoutputconfig51.ini
//
// ij_l7,0,0,0,0,0,0,0,L88 Blink fu500 fd550
// test_basic,0,0,0,0,0,0,0,L88
// test_fade,0,0,0,0,0,0,0,L88 fu500 fd550
// test_blink,0,0,0,0,0,0,0,L88 Blink
// test_both,0,0,0,0,0,0,0,L88 Blink fu500 fd550
//

#include "DOF/DOF.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstring>

#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#endif

struct TestRom
{
   std::string name;
   std::string description;
};

std::vector<TestRom> testRoms = { { "ij_l7", "IJ - L88 Blink fu500 fd550" }, { "test_basic", "Basic L88 - no effects" }, { "test_fade", "L88 fu500 fd550 - fade only" },
   { "test_blink", "L88 Blink - blink only" }, { "test_both", "L88 Blink fu500 fd550 - blink + fade" } };

void LIBDOFCALLBACK LogCallback(DOF_LogLevel logLevel, const char* format, va_list args)
{
   const char* levelStr = "";
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

void RunDOFTests(DOF::DOF* pDof)
{
   Log("=== DOF Protocol Test Scenarios ===");

   Log("1. L88 lamp test - 5 seconds on (watch PinscapePico LEDs)");
   pDof->DataReceive('L', 88, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(5000));
   Log("   L88 off - 2 seconds off");
   pDof->DataReceive('L', 88, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(2000));

   Log("2. S10 solenoid test - trigger for 3 seconds");
   pDof->DataReceive('S', 10, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(3000));
   pDof->DataReceive('S', 10, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));

   Log("3. W88 switch test - press for 3 seconds");
   pDof->DataReceive('W', 88, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(3000));
   pDof->DataReceive('W', 88, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));

   Log("=== DOF Protocol Tests Complete ===");
}

void RunL88Tests(DOF::DOF* pDof)
{
   Log("=== L88 Test Scenarios ===");

   // Test 1: Basic on/off
   Log("1. Basic L88 on/off test");
   Log("   L88 1 (should fade up + start blinking)...");
   pDof->DataReceive('L', 88, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(2000));

   Log("   L88 0 (should fade down + stop blinking)...");
   pDof->DataReceive('L', 88, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(1500));

   // Test 2: Rapid toggling
   Log("2. Rapid toggle test");
   for (int i = 0; i < 3; i++)
   {
      Log("   L88 1...");
      pDof->DataReceive('L', 88, 1);
      std::this_thread::sleep_for(std::chrono::milliseconds(300));

      Log("   L88 0...");
      pDof->DataReceive('L', 88, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
   }

   // Test 3: Direct 255 vs 1 value
   Log("3. Value comparison test");
   Log("   L88 1 (should convert to 255)...");
   pDof->DataReceive('L', 88, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));

   Log("   L88 255 (direct value)...");
   pDof->DataReceive('L', 88, 255);
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));

   Log("   L88 0 (off)...");
   pDof->DataReceive('L', 88, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));

   Log("4. Blink timing test (3 seconds)");
   pDof->DataReceive('L', 88, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(3000));

   pDof->DataReceive('L', 88, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(500));

   Log("=== L88 Tests Complete ===");
}

std::string GetDefaultBasePath()
{
#ifdef _WIN32
   char cwd[1024];
   if (getcwd(cwd, sizeof(cwd)) != nullptr)
   {
      return std::string(cwd) + "\\";
   }
   return ".\\";
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

   if (!romName.empty())
   {
      bool found = false;
      for (const auto& testRom : testRoms)
      {
         if (testRom.name == romName)
         {
            found = true;
            Log("========================================");
            Log("Testing ROM: %s - %s", testRom.name.c_str(), testRom.description.c_str());
            Log("========================================");

            DOF::DOF* pDof = new DOF::DOF();
            pDof->Init("", testRom.name.c_str());

            if (testRom.name == "ij_l7")
               RunDOFTests(pDof);
            else
               RunL88Tests(pDof);

            Log("Shutting down %s...", testRom.name.c_str());
            pDof->Finish();
            delete pDof;
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
         return 1;
      }
   }
   else
   {
      for (const auto& testRom : testRoms)
      {
         Log("========================================");
         Log("Testing ROM: %s - %s", testRom.name.c_str(), testRom.description.c_str());
         Log("========================================");

         DOF::DOF* pDof = new DOF::DOF();
         pDof->Init("", testRom.name.c_str());

         if (testRom.name == "ij_l7")
            RunDOFTests(pDof);
         else
            RunL88Tests(pDof);

         Log("Shutting down %s...", testRom.name.c_str());
         pDof->Finish();
         delete pDof;
      }

      Log("All tests completed!");
   }

   return 0;
}
