#include "Core/Config/MainSettings.h"
#include "Core/Config/SYSCONFSettings.h"
#include "DiscIO/Enums.h"
#include "DolphinLibretro/DolphinOption.h"

namespace Libretro::Options
{
class LanguageOption final : public DolphinOption<u32>
{
public:
  LanguageOption() : DolphinOption<u32>(
    "dolphin_language",
    "Language",
    Config::SYSCONF_LANGUAGE,
    {
      {"English", (u32)DiscIO::Language::English},
      {"Japanese", (u32)DiscIO::Language::Japanese},
      {"German", (u32)DiscIO::Language::German},
      {"French", (u32)DiscIO::Language::French},
      {"Spanish", (u32)DiscIO::Language::Spanish},
      {"Italian", (u32)DiscIO::Language::Italian},
      {"Dutch", (u32)DiscIO::Language::Dutch},
      {"Simplified Chinese", (u32)DiscIO::Language::SimplifiedChinese},
      {"Traditional Chinese", (u32)DiscIO::Language::TraditionalChinese},
      {"Korean", (u32)DiscIO::Language::Korean}
    }
  ) {}

private:
  virtual void UpdateDolphin(bool update = false) override
  {

    DolphinOption<u32>::UpdateDolphin(update);
    Config::SetBaseOrCurrent(Config::MAIN_GC_LANGUAGE, this->m_value - 1);
  }

};

extern LanguageOption language;
}
