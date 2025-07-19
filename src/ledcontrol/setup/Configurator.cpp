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
#include "../../fx/timmedfx/MaxDurationEffect.h"
#include "../../fx/timmedfx/MinDurationEffect.h"
#include "../../fx/timmedfx/ExtendDurationEffect.h"
#include "../../fx/timmedfx/DelayEffect.h"
#include "../../fx/timmedfx/FadeEffect.h"
#include "../../fx/valuefx/ValueInvertEffect.h"
#include <set>

#include "../../fx/valuefx/ValueMapFullRangeEffect.h"
#include "../../fx/matrixfx/RGBAMatrixColorEffect.h"
#include "../../fx/matrixfx/RGBAMatrixFlickerEffect.h"
#include "../../fx/matrixfx/RGBAMatrixPlasmaEffect.h"
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
                     if (tcs->GetColorConfig() != nullptr)
                     {
                        Log::Debug(StringExtensions::Build(
                           "Effect creation: rgbaMatrixToy, IsArea={0}, FlickerDensity={1}", tcs->IsArea() ? "true" : "false", std::to_string(tcs->GetAreaFlickerDensity())));
                        if (tcs->IsArea() && tcs->GetAreaFlickerDensity() > 0)
                        {
                           Log::Debug(StringExtensions::Build("Creating RGBAMatrixFlickerEffect for effect {0}", effectName));
                           RGBAMatrixFlickerEffect* flickerEffect = new RGBAMatrixFlickerEffect();
                           effectName = StringExtensions::Build("Ledwiz {0:00} Column {1:00} Setting {2:00} RGBAMatrixFlickerEffect", std::to_string(ledWizNr),
                              std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                           flickerEffect->SetName(effectName);
                           flickerEffect->SetToyName(toy->GetName());
                           if (tcs->HasLayer())
                              flickerEffect->SetLayerNr(tcs->GetLayer());

                           ColorConfig* colorConfig = tcs->GetColorConfig();
                           flickerEffect->SetActiveColor(RGBAColor(colorConfig->GetRed(), colorConfig->GetGreen(), colorConfig->GetBlue(), colorConfig->GetAlpha()));
                           flickerEffect->SetInactiveColor(RGBAColor(0, 0, 0, 0));
                           flickerEffect->SetDensity(tcs->GetAreaFlickerDensity());
                           flickerEffect->SetMinFlickerDurationMs(tcs->GetAreaFlickerMinDurationMs());
                           if (tcs->GetAreaFlickerMaxDurationMs() > 0)
                              flickerEffect->SetMaxFlickerDurationMs(tcs->GetAreaFlickerMaxDurationMs());
                           if (tcs->GetAreaFlickerFadeDurationMs() > 0)
                              flickerEffect->SetFadeDurationMs(tcs->GetAreaFlickerFadeDurationMs());

                           flickerEffect->SetLeft(static_cast<float>(tcs->GetAreaLeft()));
                           flickerEffect->SetTop(static_cast<float>(tcs->GetAreaTop()));
                           flickerEffect->SetWidth(static_cast<float>(tcs->GetAreaWidth()));
                           flickerEffect->SetHeight(static_cast<float>(tcs->GetAreaHeight()));

                           effect = static_cast<EffectBase*>(flickerEffect);
                        }
                        else if (tcs->IsPlasma())
                        {
                           RGBAMatrixPlasmaEffect* plasmaEffect = new RGBAMatrixPlasmaEffect();
                           effectName = StringExtensions::Build(
                              "Ledwiz {0:00} Column {1:00} Setting {2:00} RGBAMatrixPlasmaEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                           plasmaEffect->SetName(effectName);
                           plasmaEffect->SetToyName(toy->GetName());
                           if (tcs->HasLayer())
                              plasmaEffect->SetLayerNr(tcs->GetLayer());

                           ColorConfig* colorConfig = tcs->GetColorConfig();
                           RGBAColor activeColor1;
                           RGBAColor activeColor2;
                           RGBAColor inactiveColor;

                           if (colorConfig != nullptr)
                           {
                              activeColor1 = RGBAColor(colorConfig->GetRed(), colorConfig->GetGreen(), colorConfig->GetBlue(), colorConfig->GetAlpha());
                           }
                           else
                           {
                              activeColor1 = RGBAColor(0xff, 0, 0, 0xff);
                           }

                           ColorConfig* colorConfig2 = tcs->GetColorConfig2();
                           if (colorConfig2 != nullptr)
                           {
                              activeColor2 = RGBAColor(colorConfig2->GetRed(), colorConfig2->GetGreen(), colorConfig2->GetBlue(), colorConfig2->GetAlpha());
                           }
                           else
                           {
                              activeColor2 = RGBAColor(0, 0xff, 0, 0xff);
                           }

                           inactiveColor = activeColor1;
                           inactiveColor.SetAlpha(0);

                           plasmaEffect->SetActiveColor(activeColor1);
                           plasmaEffect->SetSecondaryColor(activeColor2);
                           plasmaEffect->SetInactiveColor(inactiveColor);
                           plasmaEffect->SetPlasmaSpeed(tcs->GetPlasmaSpeed());
                           plasmaEffect->SetPlasmaDensity(tcs->GetPlasmaDensity());

                           if (tcs->IsArea())
                           {
                              plasmaEffect->SetLeft(static_cast<float>(tcs->GetAreaLeft()));
                              plasmaEffect->SetTop(static_cast<float>(tcs->GetAreaTop()));
                              plasmaEffect->SetWidth(static_cast<float>(tcs->GetAreaWidth()));
                              plasmaEffect->SetHeight(static_cast<float>(tcs->GetAreaHeight()));
                           }

                           effect = static_cast<EffectBase*>(plasmaEffect);
                        }
                        else if (tcs->HasLayer() || (tcs->IsArea() && tcs->GetAreaFlickerDensity() == 0))
                        {
                           RGBAMatrixColorEffect* matrixEffect = new RGBAMatrixColorEffect();
                           effectName = StringExtensions::Build(
                              "Ledwiz {0:00} Column {1:00} Setting {2:00} RGBAMatrixColorEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                           matrixEffect->SetName(effectName);
                           matrixEffect->SetToyName(toy->GetName());
                           if (tcs->HasLayer())
                              matrixEffect->SetLayerNr(tcs->GetLayer());

                           ColorConfig* colorConfig = tcs->GetColorConfig();
                           matrixEffect->SetActiveColor(RGBAColor(colorConfig->GetRed(), colorConfig->GetGreen(), colorConfig->GetBlue(), colorConfig->GetAlpha()));
                           Log::Debug(StringExtensions::Build("Matrix effect color from config: R={0} G={1} B={2} A={3}", std::to_string(colorConfig->GetRed()),
                              std::to_string(colorConfig->GetGreen()), std::to_string(colorConfig->GetBlue()), std::to_string(colorConfig->GetAlpha())));

                           matrixEffect->SetInactiveColor(RGBAColor(0, 0, 0, 0));
                           matrixEffect->SetFadeMode(tcs->GetBlink() > 0 ? FadeModeEnum::OnOff : FadeModeEnum::Fade);

                           if (tcs->IsArea())
                           {
                              matrixEffect->SetLeft(static_cast<float>(tcs->GetAreaLeft()));
                              matrixEffect->SetTop(static_cast<float>(tcs->GetAreaTop()));
                              matrixEffect->SetWidth(static_cast<float>(tcs->GetAreaWidth()));
                              matrixEffect->SetHeight(static_cast<float>(tcs->GetAreaHeight()));
                           }

                           effect = static_cast<EffectBase*>(matrixEffect);
                        }
                        else
                        {
                           RGBAColorEffect* rgbaEffect = new RGBAColorEffect();
                           effectName = StringExtensions::Build(
                              "Ledwiz {0:00} Column {1:00} Setting {2:00} RGBAColorEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                           rgbaEffect->SetName(effectName);
                           rgbaEffect->SetToyName(toy->GetName());

                           ColorConfig* colorConfig = tcs->GetColorConfig();
                           rgbaEffect->SetActiveColor(RGBAColor(colorConfig->GetRed(), colorConfig->GetGreen(), colorConfig->GetBlue(), colorConfig->GetAlpha()));
                           rgbaEffect->SetInactiveColor(RGBAColor(0, 0, 0, 0));
                           rgbaEffect->SetFadeMode(tcs->GetBlink() > 0 ? FadeModeEnum::OnOff : FadeModeEnum::Fade);

                           effect = rgbaEffect;
                        }
                     }
                     else
                     {
                        Log::Warning(StringExtensions::Build("No color valid color definition found for area effect. Skipped setting {0} in column {1} for LedWizEqivalent number {2}",
                           std::to_string(settingNumber), std::to_string(tcc->GetNumber()), std::to_string(ledWizNr)));
                        continue;
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
                     Log::Debug(
                        StringExtensions::Build("Effect creation: rgbaToy, IsArea={0}, FlickerDensity={1}", tcs->IsArea() ? "true" : "false", std::to_string(tcs->GetAreaFlickerDensity())));
                     if (tcs->GetColorConfig() != nullptr)
                     {
                        RGBAColorEffect* rgbaEffect = new RGBAColorEffect();
                        effectName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} RGBAColorEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        rgbaEffect->SetName(effectName);
                        rgbaEffect->SetToyName(toy->GetName());

                        ColorConfig* colorConfig = tcs->GetColorConfig();
                        rgbaEffect->SetActiveColor(RGBAColor(colorConfig->GetRed(), colorConfig->GetGreen(), colorConfig->GetBlue(), colorConfig->GetAlpha()));
                        rgbaEffect->SetInactiveColor(RGBAColor(0, 0, 0, 0));
                        rgbaEffect->SetFadeMode(tcs->GetBlink() > 0 ? FadeModeEnum::OnOff : FadeModeEnum::Fade);

                        effect = rgbaEffect;
                     }
                     else
                     {
                        Log::Warning(StringExtensions::Build("Skipped setting {0} in column {1} for LedWizEqivalent number {2} since it does not contain a color specification.",
                           std::to_string(settingNumber), std::to_string(tcc->GetNumber()), std::to_string(ledWizNr)));
                        continue;
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
                     effect->SetName(StringExtensions::Build("Ledwiz {0:00} Column {1:00} Setting {2:00} " + effect->GetXmlElementName(), std::to_string(ledWizNr),
                        std::to_string(tcc->GetNumber()), std::to_string(settingNumber)));
                     MakeEffectNameUnique(effect, table);
                     table->GetEffects()->insert(std::make_pair(effect->GetName(), effect));

                     IEffect* finalEffect = effect;

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

                     if (tcs->GetBlink() != 0 && tcs->GetBlinkIntervalMsNested() > 0)
                     {
                        BlinkEffect* nestedBlinkEffect = new BlinkEffect();
                        std::string nestedBlinkName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} BlinkEffect Inner", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        nestedBlinkEffect->SetName(nestedBlinkName);
                        nestedBlinkEffect->SetTargetEffectName(finalEffect->GetName());
                        nestedBlinkEffect->SetLowValue(tcs->GetBlinkLow());

                        int nestedActiveMs = (int)((double)tcs->GetBlinkIntervalMsNested() * (double)tcs->GetBlinkPulseWidthNested() / 100);
                        int nestedInactiveMs = (int)((double)tcs->GetBlinkIntervalMsNested() * (100 - (double)tcs->GetBlinkPulseWidthNested()) / 100);
                        nestedBlinkEffect->SetDurationActiveMs(nestedActiveMs);
                        nestedBlinkEffect->SetDurationInactiveMs(nestedInactiveMs);

                        MakeEffectNameUnique(nestedBlinkEffect, table);
                        table->GetEffects()->insert(std::make_pair(nestedBlinkEffect->GetName(), nestedBlinkEffect));
                        finalEffect = nestedBlinkEffect;
                     }

                     if (tcs->GetBlink() != 0)
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

                        if (tcs->GetBlinkIntervalMsNested() == 0)
                        {
                           blinkEffect->SetLowValue(tcs->GetBlinkLow());
                        }

                        MakeEffectNameUnique(blinkEffect, table);
                        table->GetEffects()->insert(std::make_pair(blinkEffect->GetName(), blinkEffect));
                        finalEffect = blinkEffect;
                     }

                     if (tcs->GetDurationMs() > 0 || tcs->GetBlink() > 0)
                     {
                        DurationEffect* durationEffect = new DurationEffect();
                        std::string durationName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} DurationEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        durationEffect->SetName(durationName);
                        durationEffect->SetTargetEffectName(finalEffect->GetName());

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

                     if (tcs->GetMaxDurationMs() > 0)
                     {
                        MaxDurationEffect* maxDurationEffect = new MaxDurationEffect();
                        std::string maxDurationName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} MaxDurationEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        maxDurationEffect->SetName(maxDurationName);
                        maxDurationEffect->SetTargetEffectName(finalEffect->GetName());
                        maxDurationEffect->SetMaxDurationMs(tcs->GetMaxDurationMs());
                        MakeEffectNameUnique(maxDurationEffect, table);
                        table->GetEffects()->insert(std::make_pair(maxDurationEffect->GetName(), maxDurationEffect));
                        finalEffect = maxDurationEffect;
                     }

                     if (tcs->GetMinDurationMs() > 0 || (rgbaToy != nullptr && m_effectRGBMinDurationMs > 0) || (rgbaToy == nullptr && m_effectMinDurationMs > 0))
                     {
                        MinDurationEffect* minDurationEffect = new MinDurationEffect();
                        std::string effectTypeName = (tcs->GetMinDurationMs() > 0 ? "MinDurationEffect" : "DefaultMinDurationEffect");
                        int minDuration = (tcs->GetMinDurationMs() > 0 ? tcs->GetMinDurationMs() : (rgbaToy != nullptr ? m_effectRGBMinDurationMs : m_effectMinDurationMs));
                        std::string minDurationName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} {3}", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber), effectTypeName);
                        minDurationEffect->SetName(minDurationName);
                        minDurationEffect->SetTargetEffectName(finalEffect->GetName());
                        minDurationEffect->SetMinDurationMs(minDuration);
                        MakeEffectNameUnique(minDurationEffect, table);
                        table->GetEffects()->insert(std::make_pair(minDurationEffect->GetName(), minDurationEffect));
                        finalEffect = minDurationEffect;
                     }

                     if (tcs->GetExtDurationMs() > 0)
                     {
                        ExtendDurationEffect* extendDurationEffect = new ExtendDurationEffect();
                        std::string extendDurationName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} ExtendDurationEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        extendDurationEffect->SetName(extendDurationName);
                        extendDurationEffect->SetTargetEffectName(finalEffect->GetName());
                        extendDurationEffect->SetDurationMs(tcs->GetExtDurationMs());
                        MakeEffectNameUnique(extendDurationEffect, table);
                        table->GetEffects()->insert(std::make_pair(extendDurationEffect->GetName(), extendDurationEffect));
                        finalEffect = extendDurationEffect;
                     }

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

                     if (!tcs->GetNoBool())
                     {
                        ValueMapFullRangeEffect* fullRangeEffect = new ValueMapFullRangeEffect();
                        std::string fullRangeName = StringExtensions::Build(
                           "Ledwiz {0:00} Column {1:00} Setting {2:00} FullRangeEffect", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber));
                        fullRangeEffect->SetName(fullRangeName);
                        fullRangeEffect->SetTargetEffectName(finalEffect->GetName());
                        Log::Debug(StringExtensions::Build("FullRangeEffect {0} -> target {1}", fullRangeName, finalEffect->GetName()));
                        MakeEffectNameUnique(fullRangeEffect, table);
                        table->GetEffects()->insert(std::make_pair(fullRangeEffect->GetName(), fullRangeEffect));
                        finalEffect = fullRangeEffect;
                     }

                     switch (tcs->GetOutputControl())
                     {
                     case OutputControlEnum::FixedOn:
                     {
                        std::vector<std::string> tableElementDescriptors;
                        tableElementDescriptors.push_back(StringExtensions::Build("{0}.{1}.{2}", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber)));
                        AssignEffectToTableElements(table, tableElementDescriptors, finalEffect);
                     }
                     break;
                     case OutputControlEnum::Controlled:
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
                           std::vector<std::string> tableElementDescriptors;
                           tableElementDescriptors.push_back(
                              StringExtensions::Build("{0}.{1}.{2}", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber)));
                           AssignEffectToTableElements(table, tableElementDescriptors, finalEffect);
                        }
                        break;
                     case OutputControlEnum::Condition:
                     {
                        std::vector<std::string> tableElementDescriptors;
                        tableElementDescriptors.push_back(StringExtensions::Build("{0}.{1}.{2}", std::to_string(ledWizNr), std::to_string(tcc->GetNumber()), std::to_string(settingNumber)));
                        AssignEffectToTableElements(table, tableElementDescriptors, finalEffect);
                     }
                     break;
                     case OutputControlEnum::FixedOff:
                     default: break;
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

         TableElementData elementData(elementName, 0);
         tableElements->UpdateState(&elementData);

         for (TableElement* te : *tableElements)
         {
            if (te->GetName() == elementName)
            {
               tableElement = te;
               break;
            }
         }
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

               TableElementData elementData(elementType, elementNumber, 0);
               tableElements->UpdateState(&elementData);

               for (TableElement* te : *tableElements)
               {
                  if (te->GetTableElementType() == elementType && te->GetNumber() == elementNumber)
                  {
                     tableElement = te;
                     break;
                  }
               }
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

                  TableElementData elementData(elementType, number, 0);
                  Log::Write(StringExtensions::Build("Configuration: Creating table element type={0}, number={1}", std::string(1, (char)elementType), std::to_string(number)));
                  tableElements->UpdateState(&elementData);

                  for (TableElement* te : *tableElements)
                  {
                     if (te->GetTableElementType() == elementType && te->GetNumber() == number)
                     {
                        tableElement = te;
                        break;
                     }
                  }
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