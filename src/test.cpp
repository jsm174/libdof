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

int main(int argc, const char* argv[])
{
   DOF::Config* pConfig = DOF::Config::GetInstance();
   pConfig->SetLogCallback(LogCallback);
   pConfig->SetLogLevel(DOF_LogLevel_DEBUG);
   pConfig->SetBasePath("/Users/jmillard/.vpinball/");

   if (argc > 1)
   {
      std::string requestedRom = argv[1];

      bool found = false;
      for (const auto& testRom : testRoms)
      {
         if (testRom.name == requestedRom)
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
         std::string msg = "";
         for (size_t i = 0; i < testRoms.size(); i++)
         {
            msg += testRoms[i].name;
            if (i < testRoms.size() - 1)
               msg += ", ";
         }

         Log("Unknown ROM: %s", requestedRom.c_str());
         Log("Available ROMs: %s", msg.c_str());

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
