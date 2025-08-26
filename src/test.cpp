//
// libdof test app
//
// The following entries should be in ~/.vpinball/directoutputconfig/directoutputconfig51.ini:
//
// ij_l7,0,0,0,0,0,0,0,L88 Blink fu500 fd550
// tna,0,0,0,0,0,0,0,E140 Blink fu500 fd600
// gw,0,0,0,0,0,0,0,L68 m550 Blink fu500 fd550
//
// The following entries should be in ~/.vpinball/directoutputconfig/directoutputconfig30.ini:
//
// ij_l7,S10 Red AT0 AH12 L0/S20 Red AT0 AH12 L1/S24 Red AT0 AH12 L2/S52 Orange AT0 AH12 L3/S53 Red AT0 AH12 L4/W17 Orange AT0 AH12 L5/W51 Red AT0 AH12 L6/W52 Red AT0 AH12 L7/W53 Red AT0 AH12 L8/W75 Yellow AT0 AH12 L9/W76 Yellow AT0 AH12 L10/W77 Yellow AT0 AH12 L11/W78 Yellow AT0 AH12 L12/W88 Yellow AT85  AH15 FU250 FD270 BLINK 500,S9 Red AT0 AH12 L0/S51 Green AT0 AH12 L1/S53 Red AT0 AH12 L2/W16 Orange AT0 AH12 L3/W61 Red AT0 AH12 L4/W62 Red AT0 AH12 L5/W63 Red AT0 AH12 L6
// tna,E144 Yellow AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E146 Blue AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E147 Red F100 AFDEN5 AFMIN200 AFMAX300/E148 Green AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E149 Purple AL0 AT0 AW100 AH100 AFDEN50 AFMIN500 AFMAX1000/E105 Red 50 AH100 ADU AS300/E107 White 40 AT40 AL0 AH10 AW100 AS400 ADU L25/E107 White 40 AT50 AL0 AH10 AW100 AS400 ADD L25/E116 Red 700 AT85 AH15 AFDEN40 AFMIN100 AFMAX160/E125 White 50 AH100 ADU AS300/E111 Red 40 AT40 AL0 AH10 AW100 AS400 ADU L25/E111 Red 40 AT50 AL0 AH10 AW100 AS400 ADD L25/E112 Purple L13 AT0 AFDEN15 AFMIN10 AFMAX20/E179 Red 60 AW100 AH100 ADD AS300/E150|E151|E152|E153 Yellow 40 AT15 AL0 AH10 AW100 AS400 ADU L25/E150|E151|E152|E153 Yellow 40 AT25 AL0 AH10 AW100 AS400 ADD L25,E144 Yellow AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E146 Blue AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E147 Red F100 AFDEN5 AFMIN200 AFMAX300/E148 Green AL0 AT0 F100 AFDEN5 AFMIN200 AFMAX300/E149 Purple AL0 AT0 AW100 AH100 AFDEN50 AFMIN500 AFMAX1000/E103 Red 50 AH100 ADU AS300/E116 Red 700 AT85 AH15 AFDEN40 AFMIN100 AFMAX160/E108 White 40 AT40 AL0 AH10 AW100 AS400 ADU L25/E108 White 40 AT50 AL0 AH10 AW100 AS400 ADD L25/E110 Red 40 AT40 AL0 AH10 AW100 AS400 ADU L25/E110 Red 40 AT50 AL0 AH10 AW100 AS400 ADD L25/E112 Purple L13 AT0 AFDEN15 AFMIN10 AFMAX20/E179 Red 60 AW100 AH100 ADD AS300/E150|E151|E152|E153 Yellow 40 AT15 AL0 AH10 AW100 AS400 ADU L25/E150|E151|E152|E153 Yellow 40 AT25 AL0 AH10 AW100 AS400 ADD L25
// gw,S24 Yellow 50 AH100 ADU AS300/(W78=1 and (S46=1 or S48=1)) Red 500 W200 AT0 AH50 ADD AS500 L101/(W78=1 and (S46=1 or S48=1)) Black 500 W550 AT0 AH50 ADU AS500 L102/W78 Red AFDEN50 AFMIN60 AFMAX120 AT80 AH90/S16 Red 700 AT85 AH15 AFDEN40 AFMIN100 AFMAX160/W28 Red 100 AT80 AH20 ADD AS300/W27 Green 100 AT80 AH20 ADD AS300/W44|W45|W46 Orange_Red 40 AT25 AL0 AH25 AW100 AS400 ADU L25/W44|W45|W46 Orange_Red 40 AT50 AL0 AH25 AW100 AS400 ADD L25/W81 Red F150 AH30 AT0 ADD AS500 100 W450/S12 Red AT50 AH50 ADU 100 AS400/S12 Yellow AT0 AH50 ADD 100 AS400 W600 FD220/W36 Red AT20 AH5 150/W41 Yellow AT23 AH5 150/W51 Green AT26 AH5 150/W65 Blue AT0 AH50 ADD AS250 100/W65 Red AT0 AH50 ADD AS250 100 W90/W17 Green f200 AL0 AT0 AW100 AH15 L2
// goldcue,L73 Lime L0 AL0 AW100 AH2 AT87 SHPRound4,L74 Lime L0 AL0 AW100 AH2 AT89 SHPRound4,L75 Lime L0 AL0 AW100 AH2 AT91 SHPRound4,L76 Lime L0 AL0 AW100 AH2 AT93 SHPRound4,L77 Lime L0 AL0 AW100 AH2 AT95 SHPRound4,L78 Lime L0 AL0 AW100 AH2 AT97 SHPRound4,S17 Sky_blue L0 AL0 AW100 AH10 AT2 SHPRound6,S3 Red L0 AL0 AW16 AH16 AT0 SHPLetterLargeA
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

std::vector<TestRom> testRoms
   = { { "ij_l7", "Indiana Jones L7" }, { "tna", "Total Nuclear Annihilation" }, { "gw", "The Getaway High Speed II" }, { "goldcue", "Gold Cue" }, { "bourne", "Bourne Identity" } };

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
   }

   Log("Shutting down...");
   delete pDof;

   Log("All tests completed!");

   return 0;
}
