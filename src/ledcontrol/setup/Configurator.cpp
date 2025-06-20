#include "Configurator.h"
#include "../../Log.h"
#include "../../table/Table.h"
#include "../../cab/Cabinet.h"
#include "../../cab/toys/ToyList.h"
#include "../../cab/toys/IToy.h"
#include "../../cab/toys/IRGBAToy.h"
#include "../../cab/toys/IRGBOutputToy.h"
#include "../../cab/toys/IAnalogAlphaToy.h"
#include "../../cab/toys/ISingleOutputToy.h"
#include "../../cab/toys/IMatrixToy.h"
#include "../../general/analog/AnalogAlpha.h"
#include "../../cab/toys/layer/RGBAToy.h"
#include "../../cab/toys/layer/AnalogAlphaToy.h"
#include "../../cab/toys/lwequivalent/LedWizEquivalent.h"
#include "../loader/LedControlConfigList.h"
#include "../loader/LedControlConfig.h"
#include "../loader/TableConfig.h"
#include "../loader/TableConfigList.h"
#include "../loader/TableConfigColumnList.h"
#include "../loader/TableConfigColumn.h"
#include "../loader/TableConfigSetting.h"
#include "../../general/StringExtensions.h"
#include "../../fx/rgbafx/RGBAColorEffect.h"
#include "../../fx/analogtoyfx/AnalogToyValueEffect.h"
#include "../../fx/timmedfx/BlinkEffect.h"
#include "../../fx/timmedfx/DurationEffect.h"
#include "../../fx/timmedfx/DelayEffect.h"
#include "../../fx/timmedfx/FadeEffect.h"
#include "../../fx/valuefx/ValueInvertEffect.h"
#include "../../fx/valuefx/ValueMapFullRangeEffect.h"
#include "../../fx/matrixfx/RGBAMatrixColorEffect.h"
#include "../../fx/matrixfx/AnalogAlphaMatrixValueEffect.h"
#include "../../fx/nullfx/NullEffect.h"
#include "../../fx/FadeModeEnum.h"
#include "../../fx/RetriggerBehaviourEnum.h"
#include "../../fx/timmedfx/BlinkEffectUntriggerBehaviourEnum.h"
#include "../../fx/IEffect.h"
#include "../../fx/EffectList.h"
#include "../../fx/AssignedEffect.h"
#include "../../fx/AssignedEffectList.h"
#include "../../table/TableElement.h"
#include "../../table/TableElementList.h"
#include "../../table/TableElementData.h"
#include "../../table/TableElementTypeEnum.h"
#include <string>
#include <cctype>
#include <stdexcept>

namespace DOF
{

Configurator::Configurator()
   : m_effectMinDurationMs(60)
   , m_effectRGBMinDurationMs(120)
{
}

Configurator::~Configurator() { }

void Configurator::Setup(LedControlConfigList* ledControlConfigList, Table* table, Cabinet* cabinet, const std::string& romName)
{
   Log::Write(StringExtensions::Build("Configurator::Setup called for ROM '{0}'", romName));
   std::unordered_map<int, TableConfig*> tableConfigDict = ledControlConfigList->GetTableConfigDictonary(romName);
   Log::Write(StringExtensions::Build("Found {0} table configs for ROM '{1}'", std::to_string(tableConfigDict.size()), romName));

   std::string iniFilePath = "";
   if (ledControlConfigList->size() > 0)
   {
      iniFilePath = (*ledControlConfigList)[0]->GetLedControlIniFile();
      size_t lastSlash = iniFilePath.find_last_of("/\\");
      if (lastSlash != std::string::npos)
         iniFilePath = iniFilePath.substr(0, lastSlash);
   }

   std::unordered_map<int, std::unordered_map<int, IToy*>> toyAssignments = SetupCabinet(tableConfigDict, cabinet);

   SetupTable(table, tableConfigDict, toyAssignments, iniFilePath);
}

std::unordered_map<int, std::unordered_map<int, IToy*>> Configurator::SetupCabinet(const std::unordered_map<int, TableConfig*>& tableConfigDict, Cabinet* cabinet)
{
   std::unordered_map<int, std::unordered_map<int, IToy*>> toyAssignments;

   std::unordered_map<int, LedWizEquivalent*> ledWizEquivalentDict;
   for (IToy* t : *cabinet->GetToys())
   {
      LedWizEquivalent* ledWizEquivalent = dynamic_cast<LedWizEquivalent*>(t);
      if (ledWizEquivalent != nullptr)
      {
         bool found = false;
         for (const auto& kvp : ledWizEquivalentDict)
         {
            if (kvp.first == ledWizEquivalent->GetLedWizNumber())
            {
               found = true;
               break;
            }
         }
         if (!found)
            ledWizEquivalentDict[ledWizEquivalent->GetLedWizNumber()] = ledWizEquivalent;
         else
         {
            Log::Warning(StringExtensions::Build("Found more than one ledwiz with number {0}.", std::to_string(ledWizEquivalent->GetLedWizNumber())));
         }
      }
   }

   for (const auto& kv : tableConfigDict)
   {
      int ledWizNr = kv.first;
      toyAssignments[ledWizNr] = std::unordered_map<int, IToy*>();

      TableConfig* tc = kv.second;
      if (ledWizEquivalentDict.find(ledWizNr) != ledWizEquivalentDict.end())
      {
         LedWizEquivalent* lwe = ledWizEquivalentDict[ledWizNr];

         for (TableConfigColumn* tcc : *tc->GetColumns())
         {
            IToy* targetToy = nullptr;

            if (tcc->IsArea())
            {
               auto* lweOutput = lwe->GetOutputs().FindByNumber(tcc->GetFirstOutputNumber());
               if (lweOutput != nullptr)
               {
                  std::string outputName = lweOutput->GetOutputName();

                  for (IToy* toy : *cabinet->GetToys())
                  {
                     if (toy->GetName() == outputName)
                     {
                        targetToy = toy;
                        break;
                     }
                  }

                  if (targetToy == nullptr)
                  {
                     Log::Warning(StringExtensions::Build("Unknown toyname {0} defined for column {1} of LedwizEquivalent {2} (must be a matrix toy).", outputName,
                        std::to_string(tcc->GetFirstOutputNumber()), lwe->GetName()));
                  }
               }
            }
            else
            {
               switch (tcc->GetRequiredOutputCount())
               {
               case 3:
               {
                  auto* lweOutput = lwe->GetOutputs().FindByNumber(tcc->GetFirstOutputNumber());
                  if (lweOutput != nullptr)
                  {
                     std::string outputName = lweOutput->GetOutputName();

                     for (IToy* toy : *cabinet->GetToys())
                     {
                        IRGBAToy* rgbaToy = dynamic_cast<IRGBAToy*>(toy);
                        if (rgbaToy != nullptr && toy->GetName() == outputName)
                        {
                           targetToy = toy;
                           break;
                        }
                     }
                  }

                  if (targetToy == nullptr)
                  {
                     auto* lweOutput1 = lwe->GetOutputs().FindByNumber(tcc->GetFirstOutputNumber());
                     auto* lweOutput2 = lwe->GetOutputs().FindByNumber(tcc->GetFirstOutputNumber() + 1);
                     auto* lweOutput3 = lwe->GetOutputs().FindByNumber(tcc->GetFirstOutputNumber() + 2);

                     if (lweOutput1 != nullptr && lweOutput2 != nullptr && lweOutput3 != nullptr)
                     {
                        for (IToy* toy : *cabinet->GetToys())
                        {
                           IRGBOutputToy* rgbOutputToy = dynamic_cast<IRGBOutputToy*>(toy);
                           if (rgbOutputToy != nullptr && rgbOutputToy->GetOutputNameRed() == lweOutput1->GetOutputName() && rgbOutputToy->GetOutputNameGreen() == lweOutput2->GetOutputName()
                              && rgbOutputToy->GetOutputNameBlue() == lweOutput3->GetOutputName())
                           {
                              targetToy = toy;
                              break;
                           }
                        }

                        if (targetToy == nullptr)
                        {
                           std::string toyName = StringExtensions::Build("LedWiz {0:00} Column {1:00}", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()));
                           if (cabinet->GetToys()->Contains(toyName))
                           {
                              int cnt = 1;
                              while (cabinet->GetToys()->Contains(StringExtensions::Build("{0} {1}", toyName, std::to_string(cnt))))
                                 cnt++;
                              toyName = StringExtensions::Build("{0} {1}", toyName, std::to_string(cnt));
                           }

                           RGBAToy* rgbaToy = new RGBAToy();
                           rgbaToy->SetName(toyName);
                           rgbaToy->SetOutputNameRed(lweOutput1->GetOutputName());
                           rgbaToy->SetOutputNameGreen(lweOutput2->GetOutputName());
                           rgbaToy->SetOutputNameBlue(lweOutput3->GetOutputName());

                           targetToy = rgbaToy;
                           cabinet->GetToys()->AddToy(targetToy);
                        }
                     }
                  }
                  break;
               }
               case 1:
               {
                  auto* lweOutput = lwe->GetOutputs().FindByNumber(tcc->GetFirstOutputNumber());
                  if (lweOutput != nullptr)
                  {
                     std::string outputName = lweOutput->GetOutputName();

                     for (IToy* toy : *cabinet->GetToys())
                     {
                        IAnalogAlphaToy* analogToy = dynamic_cast<IAnalogAlphaToy*>(toy);
                        if (analogToy != nullptr && toy->GetName() == outputName)
                        {
                           targetToy = toy;
                           break;
                        }
                     }

                     if (targetToy == nullptr)
                     {
                        for (IToy* toy : *cabinet->GetToys())
                        {
                           ISingleOutputToy* singleOutputToy = dynamic_cast<ISingleOutputToy*>(toy);
                           if (singleOutputToy != nullptr && singleOutputToy->GetOutputName() == outputName)
                           {
                              targetToy = toy;
                              break;
                           }
                        }
                     }

                     if (targetToy == nullptr)
                     {
                        std::string toyName = StringExtensions::Build("LedWiz {0:00} Column {1:00}", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()));
                        if (cabinet->GetToys()->Contains(toyName))
                        {
                           int cnt = 1;
                           while (cabinet->GetToys()->Contains(StringExtensions::Build("{0} {1}", toyName, std::to_string(cnt))))
                              cnt++;
                           toyName = StringExtensions::Build("{0} {1}", toyName, std::to_string(cnt));
                        }

                        AnalogAlphaToy* analogAlphaToy = new AnalogAlphaToy();
                        analogAlphaToy->SetName(toyName);
                        analogAlphaToy->SetOutputName(outputName);

                        targetToy = analogAlphaToy;
                        cabinet->GetToys()->AddToy(targetToy);
                     }
                  }
                  else
                  {
                  }
                  break;
               }
               default:
                  Log::Warning(StringExtensions::Build("A illegal number ({0}) of required outputs has been found in a table config colum {1} for ledcontrol nr. {2}. Cant configure toy.",
                     std::to_string(tcc->GetRequiredOutputCount()), std::to_string(tcc->GetNumber()), std::to_string(ledWizNr)));
                  break;
               }
            }

            if (targetToy != nullptr)
               toyAssignments[ledWizNr][tcc->GetNumber()] = targetToy;
         }
      }
   }
   return toyAssignments;
}

void Configurator::SetupTable(
   Table* table, const std::unordered_map<int, TableConfig*>& tableConfigDict, const std::unordered_map<int, std::unordered_map<int, IToy*>>& toyAssignments, const std::string& iniFilePath)
{
   for (const auto& kv : tableConfigDict)
   {
      int ledWizNr = kv.first;
      Log::Write(StringExtensions::Build("Processing ledWizNr {0}", std::to_string(ledWizNr)));
      if (toyAssignments.find(ledWizNr) != toyAssignments.end())
      {
         TableConfig* tc = kv.second;
         Log::Write(StringExtensions::Build("Found TableConfig with {0} columns", std::to_string(tc->GetColumns()->size())));

         for (TableConfigColumn* tcc : *tc->GetColumns())
         {
            Log::Write(StringExtensions::Build("Processing column {0}", std::to_string(tcc->GetNumber())));
            Log::Write(StringExtensions::Build("Column {0} has {1} settings", std::to_string(tcc->GetNumber()), std::to_string(tcc->size())));

            if (toyAssignments.at(ledWizNr).find(tcc->GetNumber()) != toyAssignments.at(ledWizNr).end())
            {
               IToy* toy = toyAssignments.at(ledWizNr).at(tcc->GetNumber());
               Log::Write(StringExtensions::Build("Found toy for column {0}: {1}", std::to_string(tcc->GetNumber()), toy->GetName()));

               int settingNumber = 0;
               for (TableConfigSetting* tcs : *tcc)
               {
                  settingNumber++;

                  IEffect* effect = nullptr;
                  std::string effectName;

                  IMatrixToy<RGBAColor>* rgbaMatrixToy = dynamic_cast<IMatrixToy<RGBAColor>*>(toy);
                  IMatrixToy<AnalogAlpha>* analogMatrixToy = dynamic_cast<IMatrixToy<AnalogAlpha>*>(toy);
                  IRGBAToy* rgbaToy = dynamic_cast<IRGBAToy*>(toy);
                  IAnalogAlphaToy* analogToy = dynamic_cast<IAnalogAlphaToy*>(toy);

                  if (rgbaMatrixToy != nullptr)
                  {
                     if (tcs->HasLayer())
                     {
                        RGBAMatrixColorEffect* matrixEffect = new RGBAMatrixColorEffect();
                        effectName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} RGBAMatrixColorEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        matrixEffect->SetName(effectName);
                        matrixEffect->SetToyName(toy->GetName());
                        matrixEffect->SetLayerNr(tcs->GetLayer());

                        if (tcs->GetColorConfig() != nullptr)
                        {
                           ColorConfig* colorConfig = tcs->GetColorConfig();
                           matrixEffect->SetActiveColor(RGBAColor(colorConfig->GetRed(), colorConfig->GetGreen(), colorConfig->GetBlue(), colorConfig->GetAlpha()));
                           Log::Write(StringExtensions::Build("Matrix effect color from config: R={0} G={1} B={2} A={3}", std::to_string(colorConfig->GetRed()),
                              std::to_string(colorConfig->GetGreen()), std::to_string(colorConfig->GetBlue()), std::to_string(colorConfig->GetAlpha())));
                        }
                        else
                        {
                           matrixEffect->SetActiveColor(RGBAColor(255, 0, 0, 255)); // Set to Red for testing
                           Log::Write("Matrix effect color: Using default Red (255,0,0,255)");
                        }

                        matrixEffect->SetInactiveColor(RGBAColor(0, 0, 0, 0));
                        matrixEffect->SetFadeMode(tcs->GetBlink() > 0 ? FadeModeEnum::OnOff : FadeModeEnum::Fade);

                        effect = static_cast<EffectBase*>(matrixEffect);
                     }
                     else
                     {
                        RGBAColorEffect* rgbaEffect = new RGBAColorEffect();
                        effectName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} RGBAColorEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        rgbaEffect->SetName(effectName);
                        rgbaEffect->SetToyName(toy->GetName());

                        if (tcs->GetColorConfig() != nullptr)
                        {
                           ColorConfig* colorConfig = tcs->GetColorConfig();
                           rgbaEffect->SetActiveColor(RGBAColor(colorConfig->GetRed(), colorConfig->GetGreen(), colorConfig->GetBlue(), colorConfig->GetAlpha()));
                        }
                        else
                           rgbaEffect->SetActiveColor(RGBAColor(255, 255, 255, 255));

                        rgbaEffect->SetInactiveColor(RGBAColor(0, 0, 0, 0));
                        rgbaEffect->SetFadeMode(tcs->GetBlink() > 0 ? FadeModeEnum::OnOff : FadeModeEnum::Fade);

                        effect = rgbaEffect;
                     }
                  }
                  else if (analogMatrixToy != nullptr)
                  {
                     if (tcs->HasLayer())
                     {
                        AnalogAlphaMatrixValueEffect* matrixEffect = new AnalogAlphaMatrixValueEffect();
                        effectName = StringExtensions::Build("Ledwiz {0:00} Column {1:00} Setting {2:00} AnalogAlphaMatrixValueEffect", std::to_string(ledWizNr),
                           std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        matrixEffect->SetName(effectName);
                        matrixEffect->SetToyName(toy->GetName());
                        matrixEffect->SetLayerNr(tcs->GetLayer());

                        effect = static_cast<EffectBase*>(matrixEffect);
                     }
                     else
                     {
                        AnalogToyValueEffect* analogEffect = new AnalogToyValueEffect();
                        effectName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} AnalogToyValueEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        analogEffect->SetName(effectName);
                        analogEffect->SetToyName(toy->GetName());

                        effect = analogEffect;
                     }
                  }
                  else if (rgbaToy != nullptr)
                  {
                     if (tcs->HasLayer())
                     {
                        RGBAColorEffect* rgbaEffect = new RGBAColorEffect();
                        effectName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} RGBAColorEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        rgbaEffect->SetName(effectName);
                        rgbaEffect->SetToyName(toy->GetName());

                        if (tcs->GetColorConfig() != nullptr)
                        {
                           ColorConfig* colorConfig = tcs->GetColorConfig();
                           rgbaEffect->SetActiveColor(RGBAColor(colorConfig->GetRed(), colorConfig->GetGreen(), colorConfig->GetBlue(), colorConfig->GetAlpha()));
                        }
                        else
                           rgbaEffect->SetActiveColor(RGBAColor(255, 255, 255, 255));

                        rgbaEffect->SetInactiveColor(RGBAColor(0, 0, 0, 0));
                        rgbaEffect->SetFadeMode(tcs->GetBlink() > 0 ? FadeModeEnum::OnOff : FadeModeEnum::Fade);

                        effect = rgbaEffect;
                     }
                     else
                     {
                        RGBAColorEffect* rgbaEffect = new RGBAColorEffect();
                        effectName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} RGBAColorEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        rgbaEffect->SetName(effectName);
                        rgbaEffect->SetToyName(toy->GetName());

                        if (tcs->GetColorConfig() != nullptr)
                        {
                           ColorConfig* colorConfig = tcs->GetColorConfig();
                           rgbaEffect->SetActiveColor(RGBAColor(colorConfig->GetRed(), colorConfig->GetGreen(), colorConfig->GetBlue(), colorConfig->GetAlpha()));
                        }
                        else
                           rgbaEffect->SetActiveColor(RGBAColor(255, 255, 255, 255));

                        rgbaEffect->SetInactiveColor(RGBAColor(0, 0, 0, 0));
                        rgbaEffect->SetFadeMode(tcs->GetBlink() > 0 ? FadeModeEnum::OnOff : FadeModeEnum::Fade);

                        effect = rgbaEffect;
                     }
                  }
                  else if (analogToy != nullptr)
                  {
                     AnalogToyValueEffect* analogEffect = new AnalogToyValueEffect();
                     effectName = StringExtensions::Build(
                        "Ledwiz {0:00} Column {1:00} Setting {2:00} AnalogToyValueEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                     analogEffect->SetName(effectName);
                     analogEffect->SetToyName(toy->GetName());

                     effect = analogEffect;
                  }

                  if (effect != nullptr)
                  {
                     // 1. Add base effect immediately (matching C# pattern)
                     effect->SetName(StringExtensions::Build("Ledwiz {0:00} Column {1:00} Setting {2:00} " + effect->GetXmlElementName(), std::to_string(ledWizNr),
                        std::to_string(tcc->GetNumber()), std::to_string(settingNumber)));
                     MakeEffectNameUnique(effect, table);
                     table->GetEffects()->insert(std::make_pair(effect->GetName(), effect));

                     IEffect* finalEffect = effect;

                     // 2. FadeEffect (if fading durations are set)
                     if (tcs->GetFadingUpDurationMs() > 0 || tcs->GetFadingDownDurationMs() > 0)
                     {
                        FadeEffect* fadeEffect = new FadeEffect();
                        std::string fadeName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} FadeEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        fadeEffect->SetName(fadeName);
                        fadeEffect->SetTargetEffectName(finalEffect->GetName());
                        fadeEffect->SetFadeDownDuration(tcs->GetFadingDownDurationMs());
                        fadeEffect->SetFadeUpDuration(tcs->GetFadingUpDurationMs());
                        MakeEffectNameUnique(fadeEffect, table);
                        table->GetEffects()->insert(std::make_pair(fadeEffect->GetName(), fadeEffect));
                        finalEffect = fadeEffect;
                     }

                     // 3. BlinkEffect (if blink is enabled)
                     if (tcs->GetBlink())
                     {
                        BlinkEffect* blinkEffect = new BlinkEffect();
                        std::string blinkName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} BlinkEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        blinkEffect->SetName(blinkName);
                        blinkEffect->SetTargetEffectName(finalEffect->GetName());

                        int activeMs = (int)((double)tcs->GetBlinkIntervalMs() * (double)tcs->GetBlinkPulseWidth() / 100);
                        int inactiveMs = (int)((double)tcs->GetBlinkIntervalMs() * (100 - (double)tcs->GetBlinkPulseWidth()) / 100);
                        blinkEffect->SetDurationActiveMs(activeMs);
                        blinkEffect->SetDurationInactiveMs(inactiveMs);
                        blinkEffect->SetUntriggerBehaviour(BlinkEffectUntriggerBehaviourEnum::Immediate);

                        MakeEffectNameUnique(blinkEffect, table);
                        table->GetEffects()->insert(std::make_pair(blinkEffect->GetName(), blinkEffect));
                        finalEffect = blinkEffect;
                     }

                     // 4. DurationEffect (if duration or positive blink count is set)
                     if (tcs->GetDurationMs() > 0 || tcs->GetBlink() > 0)
                     {
                        DurationEffect* durationEffect = new DurationEffect();
                        std::string durationName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} DurationEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        durationEffect->SetName(durationName);
                        durationEffect->SetTargetEffectName(finalEffect->GetName());

                        // Calculate duration like C# version
                        int duration;
                        if (tcs->GetDurationMs() > 0)
                        {
                           duration = tcs->GetDurationMs();
                        }
                        else if (tcs->GetBlink() > 0)
                        {
                           duration = (tcs->GetBlink() * 2 - 1) * tcs->GetBlinkIntervalMs() / 2 + 1;
                        }
                        else
                        {
                           duration = (rgbaToy != nullptr) ? m_effectRGBMinDurationMs : m_effectMinDurationMs;
                        }
                        durationEffect->SetDurationMs(duration);
                        durationEffect->SetRetriggerBehaviour(RetriggerBehaviourEnum::Restart);

                        MakeEffectNameUnique(durationEffect, table);
                        table->GetEffects()->insert(std::make_pair(durationEffect->GetName(), durationEffect));
                        finalEffect = durationEffect;
                     }

                     // 5. DelayEffect (if wait duration is set)
                     if (tcs->GetWaitDurationMs() > 0)
                     {
                        DelayEffect* delayEffect = new DelayEffect();
                        std::string delayName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} DelayEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        delayEffect->SetName(delayName);
                        delayEffect->SetTargetEffectName(finalEffect->GetName());
                        delayEffect->SetDelayMs(tcs->GetWaitDurationMs());
                        MakeEffectNameUnique(delayEffect, table);
                        table->GetEffects()->insert(std::make_pair(delayEffect->GetName(), delayEffect));
                        finalEffect = delayEffect;
                     }

                     // 6. ValueInvertEffect (if invert is enabled)
                     if (tcs->GetInvert())
                     {
                        ValueInvertEffect* invertEffect = new ValueInvertEffect();
                        std::string invertName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} ValueInvertEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        invertEffect->SetName(invertName);
                        invertEffect->SetTargetEffectName(finalEffect->GetName());
                        MakeEffectNameUnique(invertEffect, table);
                        table->GetEffects()->insert(std::make_pair(invertEffect->GetName(), invertEffect));
                        finalEffect = invertEffect;
                     }

                     // 7. ValueMapFullRangeEffect (if NoBool is false)
                     if (!tcs->GetNoBool())
                     {
                        ValueMapFullRangeEffect* fullRangeEffect = new ValueMapFullRangeEffect();
                        std::string fullRangeName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} FullRangeEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        fullRangeEffect->SetName(fullRangeName);
                        fullRangeEffect->SetTargetEffectName(finalEffect->GetName());
                        MakeEffectNameUnique(fullRangeEffect, table);
                        table->GetEffects()->insert(std::make_pair(fullRangeEffect->GetName(), fullRangeEffect));
                        finalEffect = fullRangeEffect;
                     }

                     // Assign effect to table elements based on output control
                     switch (tcs->GetOutputControl())
                     {
                     case OutputControlEnum::FixedOn:
                        // For fixed on effects, this would go to assigned static effects in full implementation
                        // For now, create a synthetic descriptor as fallback
                        {
                           std::vector<std::string> tableElementDescriptors;
                           tableElementDescriptors.push_back(
                              StringExtensions::Build("{0}.{1}.{2}", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber)));
                           AssignEffectToTableElements(table, tableElementDescriptors, finalEffect);
                        }
                        break;
                     case OutputControlEnum::Controlled:
                        // Use the actual table element from the setting (e.g., "L88")
                        if (!StringExtensions::IsNullOrWhiteSpace(tcs->GetTableElement()))
                        {
                           std::vector<std::string> tableElements = StringExtensions::Split(tcs->GetTableElement(), { '|' });
                           std::vector<std::string> cleanedElements;
                           for (const std::string& element : tableElements)
                           {
                              std::string trimmed = StringExtensions::Trim(element);
                              if (!trimmed.empty())
                              {
                                 cleanedElements.push_back(trimmed);
                              }
                           }
                           AssignEffectToTableElements(table, cleanedElements, finalEffect);
                        }
                        else
                        {
                           // Fallback to synthetic descriptor if no table element specified
                           std::vector<std::string> tableElementDescriptors;
                           tableElementDescriptors.push_back(
                              StringExtensions::Build("{0}.{1}.{2}", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber)));
                           AssignEffectToTableElements(table, tableElementDescriptors, finalEffect);
                        }
                        break;
                     case OutputControlEnum::Condition:
                        // For condition effects, this would use condition parsing in full implementation
                        // For now, create a synthetic descriptor as fallback
                        {
                           std::vector<std::string> tableElementDescriptors;
                           tableElementDescriptors.push_back(
                              StringExtensions::Build("{0}.{1}.{2}", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber)));
                           AssignEffectToTableElements(table, tableElementDescriptors, finalEffect);
                        }
                        break;
                     case OutputControlEnum::FixedOff:
                     default:
                        // Don't assign fixed off effects
                        break;
                     }
                  }
               }
            }
         }
      }
   }
}

void Configurator::MakeEffectNameUnique(IEffect* effect, Table* table)
{
   if (effect == nullptr || table == nullptr)
      return;

   EffectList* effects = table->GetEffects();
   if (effects == nullptr)
      return;

   std::string originalName = effect->GetName();

   if (effects->find(originalName) != effects->end())
   {
      int cnt = 1;
      std::string newName;

      do
      {
         newName = StringExtensions::Build("{0} {1}", originalName, std::to_string(cnt));
         cnt++;
      } while (effects->find(newName) != effects->end());

      effect->SetName(newName);
   }
}

void Configurator::AssignEffectToTableElements(Table* table, const std::vector<std::string>& tableElementDescriptors, IEffect* effect)
{
   if (table == nullptr || effect == nullptr)
      return;

   TableElementList* tableElements = table->GetTableElements();
   if (tableElements == nullptr)
      return;

   Log::Write(StringExtensions::Build("AssignEffectToTableElements: Processing {0} descriptors for effect '{1}'", std::to_string(tableElementDescriptors.size()), effect->GetName()));

   for (const std::string& descriptor : tableElementDescriptors)
   {
      Log::Write(StringExtensions::Build("Processing table element descriptor: '{0}'", descriptor));
      if (descriptor.empty())
         continue;

      TableElement* tableElement = nullptr;

      if (descriptor[0] == '$')
      {
         std::string elementName = descriptor.substr(1);

         TableElementData* elementData = new TableElementData(elementName, 0);
         tableElements->UpdateState(elementData);

         for (TableElement* te : *tableElements)
         {
            if (te->GetName() == elementName)
            {
               tableElement = te;
               break;
            }
         }

         delete elementData;
      }
      else if (descriptor.find('.') != std::string::npos)
      {
         std::vector<std::string> parts = StringExtensions::Split(descriptor, { '.' });
         if (parts.size() == 3)
         {
            try
            {
               int ledWizNumber = std::stoi(parts[0]);
               int columnNumber = std::stoi(parts[1]);
               int settingNumber = std::stoi(parts[2]);

               TableElementTypeEnum elementType = TableElementTypeEnum::Solenoid;
               int elementNumber = ledWizNumber * 100 + columnNumber;

               TableElementData* elementData = new TableElementData(elementType, elementNumber, 0);
               tableElements->UpdateState(elementData);

               for (TableElement* te : *tableElements)
               {
                  if (te->GetTableElementType() == elementType && te->GetNumber() == elementNumber)
                  {
                     tableElement = te;
                     break;
                  }
               }

               delete elementData;
            }
            catch (const std::exception&)
            {
               continue;
            }
         }
      }
      else
      {
         if (descriptor.length() > 1)
         {
            char typeChar = descriptor[0];
            std::string numberStr = descriptor.substr(1);

            TableElementTypeEnum elementType = TableElementTypeEnum::Unknown;
            bool validType = false;

            switch (typeChar)
            {
            case 'L':
               elementType = TableElementTypeEnum::Lamp;
               validType = true;
               break;
            case 'W':
               elementType = TableElementTypeEnum::Switch;
               validType = true;
               break;
            case 'S':
               elementType = TableElementTypeEnum::Solenoid;
               validType = true;
               break;
            case 'G':
               elementType = TableElementTypeEnum::GIString;
               validType = true;
               break;
            case 'M':
               elementType = TableElementTypeEnum::Mech;
               validType = true;
               break;
            case 'N':
               elementType = TableElementTypeEnum::GetMech;
               validType = true;
               break;
            case 'E':
               elementType = TableElementTypeEnum::EMTable;
               validType = true;
               break;
            case 'D':
               elementType = TableElementTypeEnum::LED;
               validType = true;
               break;
            case 'C':
               elementType = TableElementTypeEnum::Score;
               validType = true;
               break;
            case 'B':
               elementType = TableElementTypeEnum::ScoreDigit;
               validType = true;
               break;
            default: validType = false; break;
            }

            if (validType)
            {
               try
               {
                  int number = std::stoi(numberStr);

                  TableElementData* elementData = new TableElementData(elementType, number, 0);
                  Log::Write(StringExtensions::Build("Configuration: Creating table element type={0}, number={1}", std::string(1, (char)elementType), std::to_string(number)));
                  tableElements->UpdateState(elementData);

                  for (TableElement* te : *tableElements)
                  {
                     if (te->GetTableElementType() == elementType && te->GetNumber() == number)
                     {
                        tableElement = te;
                        break;
                     }
                  }

                  delete elementData;
               }
               catch (const std::exception&)
               {
                  continue;
               }
            }
         }
      }

      if (tableElement != nullptr)
      {
         AssignedEffectList* assignedEffects = tableElement->GetAssignedEffects();
         if (assignedEffects != nullptr)
         {
            Log::Write(StringExtensions::Build("Configuration: Adding effect {0} to table element type={1}, number={2}", effect->GetName(),
               std::string(1, (char)tableElement->GetTableElementType()), std::to_string(tableElement->GetNumber())));
            assignedEffects->Add(effect->GetName());
         }
      }
   }
}

}