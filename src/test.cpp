//
// libdof test app
//
// The following entries should be in ~/.vpinball/directoutputconfig/directoutputconfig51.ini:
//
// ij_l7,0,0,0,0,0,0,0,L88 Blink fu500 fd550
// tna,0,0,0,0,0,0,0,E140 Blink fu500 fd600
//
// The following entries should be in ~/.vpinball/directoutputconfig/directoutputconfig30.ini:
//
// ij_l7,S10 Red AT0 AH12 L0/S20 Red AT0 AH12 L1/S24 Red AT0 AH12 L2/S52 Orange AT0 AH12 L3/S53 Red AT0 AH12 L4/W17 Orange AT0 AH12 L5/W51 Red AT0 AH12 L6/W52 Red AT0 AH12 L7/W53 Red AT0 AH12 L8/W75 Yellow AT0 AH12 L9/W76 Yellow AT0 AH12 L10/W77 Yellow AT0 AH12 L11/W78 Yellow AT0 AH12 L12/W88 Yellow AT85  AH15 FU250 FD270 BLINK 500,S9 Red AT0 AH12 L0/S51 Green AT0 AH12 L1/S53 Red AT0 AH12 L2/W16 Orange AT0 AH12 L3/W61 Red AT0 AH12 L4/W62 Red AT0 AH12 L5/W63 Red AT0 AH12 L6
// tna,E144 Yellow AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E146 Blue AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E147 Red F100 AFDEN5 AFMIN200 AFMAX300/E148 Green AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E149 Purple AL0 AT0 AW100 AH100 AFDEN50 AFMIN500 AFMAX1000/E105 Red 50 AH100 ADU AS300/E107 White 40 AT40 AL0 AH10 AW100 AS400 ADU L25/E107 White 40 AT50 AL0 AH10 AW100 AS400 ADD L25/E116 Red 700 AT85 AH15 AFDEN40 AFMIN100 AFMAX160/E125 White 50 AH100 ADU AS300/E111 Red 40 AT40 AL0 AH10 AW100 AS400 ADU L25/E111 Red 40 AT50 AL0 AH10 AW100 AS400 ADD L25/E112 Purple L13 AT0 AFDEN15 AFMIN10 AFMAX20/E179 Red 60 AW100 AH100 ADD AS300/E150|E151|E152|E153 Yellow 40 AT15 AL0 AH10 AW100 AS400 ADU L25/E150|E151|E152|E153 Yellow 40 AT25 AL0 AH10 AW100 AS400 ADD L25,E144 Yellow AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E146 Blue AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E147 Red F100 AFDEN5 AFMIN200 AFMAX300/E148 Green AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E149 Purple AL0 AT0 AW100 AH100 AFDEN50 AFMIN500 AFMAX1000/E103 Red 50 AH100 ADU AS300/E116 Red 700 AT85 AH15 AFDEN40 AFMIN100 AFMAX160/E108 White 40 AT40 AL0 AH10 AW100 AS400 ADU L25/E108 White 40 AT50 AL0 AH10 AW100 AS400 ADD L25/E110 Red 40 AT40 AL0 AH10 AW100 AS400 ADU L25/E110 Red 40 AT50 AL0 AH10 AW100 AS400 ADD L25/E112 Purple L13 AT0 AFDEN15 AFMIN10 AFMAX20/E179 Red 60 AW100 AH100 ADD AS300/E150|E151|E152|E153 Yellow 40 AT15 AL0 AH10 AW100 AS400 ADU L25/E150|E151|E152|E153 Yellow 40 AT25 AL0 AH10 AW100 AS400 ADD L25
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

std::vector<TestRom> testRoms = { { "ij_l7", "" }, { "tna", "" } };

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

void RunIJTests(DOF::DOF* pDof)
{
   Log("========================================");
   Log("Testing ROM: ij_l7 - Indiana Jones L7");
   Log("========================================");
   pDof->Init("", "ij_l7");

   Log("=== DOF Protocol Test Scenarios ===");

   Log("1. L88 lamp test - 5 seconds on");
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
   pDof->Finish();
}

void RunTNATests(DOF::DOF* pDof)
{
   Log("========================================");
   Log("Testing ROM: tna - Total Nuclear Annihilation");
   Log("========================================");
   pDof->Init("", "tna");

   Log("=== TNA Matrix Effects Test Scenarios ===");

   std::this_thread::sleep_for(std::chrono::milliseconds(1000));

   Log("1. Run E105 for 5 seconds ON");

   pDof->DataReceive('E', 105, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 5));

   Log("2. Run E105 for 5 seconds OFF");

   pDof->DataReceive('E', 105, 0);

   Log("3. Run E149 for 5 seconds ON");

   pDof->DataReceive('E', 149, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 5));

   Log("4. Run E149 for 5 seconds OFF");

   pDof->DataReceive('E', 149, 0);

   std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 5));

   Log("5. Run sample of events");

   pDof->DataReceive('E', 140, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(2));
   pDof->DataReceive('E', 146, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(0));
   pDof->DataReceive('E', 149, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(1));
   pDof->DataReceive('E', 1, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(2351));
   pDof->DataReceive('E', 140, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(716));
   pDof->DataReceive('E', 149, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(2));
   pDof->DataReceive('E', 146, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(0));
   pDof->DataReceive('C', 1, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(3));
   pDof->DataReceive('E', 196, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(0));
   pDof->DataReceive('E', 196, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(2));
   pDof->DataReceive('E', 146, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(0));
   pDof->DataReceive('E', 1, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(494));
   pDof->DataReceive('E', 146, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(1));
   pDof->DataReceive('E', 145, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(0));
   pDof->DataReceive('E', 145, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(29));
   pDof->DataReceive('E', 112, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(0));
   pDof->DataReceive('E', 112, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(0));
   pDof->DataReceive('E', 112, 1);
   std::this_thread::sleep_for(std::chrono::milliseconds(0));
   pDof->DataReceive('E', 112, 0);
   std::this_thread::sleep_for(std::chrono::milliseconds(0));

   Log("=== TNA Matrix Effects Tests Complete ===");
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
   }

   Log("Shutting down...");
   delete pDof;

   Log("All tests completed!");

   return 0;
}
