#include "DirectStripController.h"
#include "LedStripOutput.h"
#include "../../Cabinet.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include "../../../general/MathExtensions.h"
#include <algorithm>
#include <cstring>
#include <future>

namespace DOF
{

const std::vector<int> DirectStripController::s_colNrLookup = { 1, 0, 2 };

DirectStripController::DirectStripController()
   : m_controllerNumber(1)
   , m_numberOfLeds(1)
   , m_packData(false)
   , m_updateRequired(true)
   , m_updaterThread(nullptr)
   , m_keepUpdaterThreadAlive(false)
   , m_controller(nullptr)
{
   SetNumberOfLeds(1);
}

DirectStripController::~DirectStripController()
{
   FinishUpdaterThread();

   if (m_controller)
   {
      delete m_controller;
      m_controller = nullptr;
   }
}

void DirectStripController::SetNumberOfLeds(int value)
{
   m_numberOfLeds = MathExtensions::Limit(value, 0, 4006);
   m_ledData.resize(m_numberOfLeds * 3, 0);
   m_outputLedData.resize(m_numberOfLeds * 3, 0);
}

void DirectStripController::SetValues(int firstOutput, const uint8_t* values, int valueCount)
{
   if (firstOutput >= static_cast<int>(m_ledData.size()) || firstOutput < 0 || !values || valueCount <= 0)
      return;

   int copyLength = MathExtensions::Limit(valueCount, 0, static_cast<int>(m_ledData.size()) - firstOutput);
   if (copyLength < 1)
      return;

   std::lock_guard<std::mutex> lock(m_updateLocker);
   std::memcpy(&m_ledData[firstOutput], values, copyLength);
   m_updateRequired = true;
}

void DirectStripController::OnOutputValueChanged(IOutput* output)
{
   if (!output)
      return;

   int byteNr = output->GetNumber() - 1;
   byteNr = (byteNr / 3) * 3 + s_colNrLookup[byteNr % 3];

   try
   {
      std::lock_guard<std::mutex> lock(m_updateLocker);

      if (byteNr >= 0 && byteNr < static_cast<int>(m_ledData.size()))
      {
         if (m_ledData[byteNr] != output->GetOutput())
         {
            m_ledData[byteNr] = output->GetOutput();
            m_updateRequired = true;
         }
      }
   }
   catch (const std::exception& e)
   {
      Log::Exception(StringExtensions::Build("DirectStripController {0} with number {1} has received a update with a illegal or to high output number ({2}).", GetName(),
         std::to_string(m_controllerNumber), std::to_string(output->GetNumber())));
   }
}

void DirectStripController::AddOutputs()
{
   if (!GetOutputs())
      SetOutputs(new OutputList());

   auto* outputList = GetOutputs();

   for (int i = 1; i <= m_numberOfLeds * 3; i++)
   {
      bool found = false;
      for (size_t j = 0; j < outputList->size(); ++j)
      {
         if ((*outputList)[j] && (*outputList)[j]->GetNumber() == i)
         {
            found = true;
            break;
         }
      }

      if (!found)
      {
         LedStripOutput* output = new LedStripOutput();
         output->SetNumber(i);
         output->SetName(StringExtensions::Build("{0}.{1}", GetName(), std::to_string(i)));
         outputList->Add(output);
      }
   }
}

void DirectStripController::Init(Cabinet* cabinet)
{
   AddOutputs();
   InitUpdaterThread();
   Log::Write(StringExtensions::Build("DirectStripController {0} with number {1} initialized and updaterthread started.", GetName(), std::to_string(m_controllerNumber)));
}

void DirectStripController::Finish()
{
   FinishUpdaterThread();
   Log::Write(StringExtensions::Build("DirectStripController {0} with number {1} finished and updaterthread stopped.", GetName(), std::to_string(m_controllerNumber)));
}

void DirectStripController::Update()
{
   if (m_updateRequired)
      UpdaterThreadSignal();
}

void DirectStripController::InitUpdaterThread()
{
   if (!IsUpdaterThreadActive())
   {
      m_keepUpdaterThreadAlive = true;
      try
      {
         m_updaterThread = new std::thread(&DirectStripController::UpdaterThreadDoIt, this);
      }
      catch (const std::exception& e)
      {
         Log::Exception(StringExtensions::Build("DirectStripController {0} named {1} updater thread could not start.", std::to_string(m_controllerNumber), GetName()));
         throw;
      }
   }
}

void DirectStripController::FinishUpdaterThread()
{
   if (m_updaterThread)
   {
      try
      {
         m_keepUpdaterThreadAlive = false;
         UpdaterThreadSignal();

         auto future = std::async(std::launch::async, [this]() { m_updaterThread->join(); });

         if (future.wait_for(std::chrono::milliseconds(1000)) == std::future_status::timeout)
         {
            Log::Warning("DirectStripController updater thread did not quit within timeout.");
         }

         delete m_updaterThread;
         m_updaterThread = nullptr;
      }
      catch (const std::exception& e)
      {
         Log::Exception("A error occurred during termination of DirectStripController updater thread.");
      }
   }
}

void DirectStripController::UpdaterThreadSignal()
{
   std::lock_guard<std::mutex> lock(m_conditionMutex);
   m_updateCondition.notify_one();
}

bool DirectStripController::IsUpdaterThreadActive() const { return m_updaterThread && m_updaterThread->joinable(); }

void DirectStripController::UpdaterThreadDoIt()
{
   if (!m_controller)
   {
      m_controller = new DirectStripControllerApi(m_controllerNumber);
      if (!m_controller->GetDeviceIsPresent())
      {
         Log::Warning(StringExtensions::Build("WS2811 Strip Controller Nr. {0} is not present. Will not send updates.", std::to_string(m_controllerNumber)));
         delete m_controller;
         m_controller = nullptr;
      }
   }

   std::fill(m_outputLedData.begin(), m_outputLedData.end(), 0);
   if (m_controller)
   {
      if (m_packData)
         m_controller->SetAndDisplayPackedData(m_outputLedData);
      else
         m_controller->SetAndDisplayData(m_outputLedData);
   }

   while (m_keepUpdaterThreadAlive)
   {
      if (m_updateRequired)
      {
         {
            std::lock_guard<std::mutex> lock(m_updateLocker);
            m_updateRequired = false;
            std::memcpy(m_outputLedData.data(), m_ledData.data(), m_ledData.size());
         }

         if (m_controller)
         {
            if (m_packData)
               m_controller->SetAndDisplayPackedData(m_outputLedData);
            else
               m_controller->SetAndDisplayData(m_outputLedData);
         }
      }

      if (m_keepUpdaterThreadAlive)
      {
         std::unique_lock<std::mutex> lock(m_conditionMutex);
         m_updateCondition.wait_for(lock, std::chrono::milliseconds(50), [this] { return !m_keepUpdaterThreadAlive || m_updateRequired; });
      }
   }

   std::fill(m_outputLedData.begin(), m_outputLedData.end(), 0);
   if (m_controller)
   {
      if (m_packData)
         m_controller->SetAndDisplayPackedData(m_outputLedData);
      else
         m_controller->SetAndDisplayData(m_outputLedData);

      m_controller->Close();
      delete m_controller;
      m_controller = nullptr;
   }
}

bool DirectStripController::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   const char* name = element->Attribute("Name");
   if (name)
      SetName(name);

   const tinyxml2::XMLElement* controllerNumberElement = element->FirstChildElement("ControllerNumber");
   if (controllerNumberElement && controllerNumberElement->GetText())
   {
      try
      {
         int controllerNumber = std::stoi(controllerNumberElement->GetText());
         SetControllerNumber(controllerNumber);
      }
      catch (...)
      {
         return false;
      }
   }

   const tinyxml2::XMLElement* numberOfLedsElement = element->FirstChildElement("NumberOfLeds");
   if (numberOfLedsElement && numberOfLedsElement->GetText())
   {
      try
      {
         int numberOfLeds = std::stoi(numberOfLedsElement->GetText());
         SetNumberOfLeds(numberOfLeds);
      }
      catch (...)
      {
         return false;
      }
   }

   const tinyxml2::XMLElement* packDataElement = element->FirstChildElement("PackData");
   if (packDataElement && packDataElement->GetText())
   {
      SetPackData(std::string(packDataElement->GetText()) == "true");
   }

   return true;
}

tinyxml2::XMLElement* DirectStripController::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   if (!GetName().empty())
      element->SetAttribute("Name", GetName().c_str());

   tinyxml2::XMLElement* controllerNumberElement = doc.NewElement("ControllerNumber");
   controllerNumberElement->SetText(m_controllerNumber);
   element->InsertEndChild(controllerNumberElement);

   tinyxml2::XMLElement* numberOfLedsElement = doc.NewElement("NumberOfLeds");
   numberOfLedsElement->SetText(m_numberOfLeds);
   element->InsertEndChild(numberOfLedsElement);

   tinyxml2::XMLElement* packDataElement = doc.NewElement("PackData");
   packDataElement->SetText(m_packData ? "true" : "false");
   element->InsertEndChild(packDataElement);

   return element;
}

}