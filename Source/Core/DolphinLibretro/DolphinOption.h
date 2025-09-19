#pragma once
#include "Common/Config/Config.h"
#include "Common/Config/ConfigInfo.h"
#include "Core/PowerPC/PowerPC.h"
#include "DolphinLibretro/Options.h"
#include "DolphinLibretro/OptionUpdater.h"
#include "VideoCommon/VideoConfig.h"

namespace Libretro
{
namespace Options
{
extern std::vector<OptionUpdater*> dolphin_options;

template <typename T>
class DolphinOption : public Option<T>, public OptionUpdater
{
public:
  DolphinOption(
    const char* id,
    const char* name,
    Config::Info<T> dolphin_info,
    std::initializer_list<std::pair<const char*, T>> list
  ) : Option<T>(id, name, list), m_dolphin_info(dolphin_info) { dolphin_options.push_back(this); }
  DolphinOption(
    const char* id,
    const char* name,
    Config::Info<T> dolphin_info,
    std::initializer_list<const char*> list
  ) : Option<T>(id, name, list), m_dolphin_info(dolphin_info) { dolphin_options.push_back(this); }
  DolphinOption(
    const char* id,
    const char* name,
    Config::Info<T> dolphin_info,
    T first,
    std::initializer_list<const char*> list
  ) : Option<T>(id, name, first, list), m_dolphin_info(dolphin_info) { dolphin_options.push_back(this); }
  DolphinOption(
    const char* id,
    const char* name,
    Config::Info<T> dolphin_info,
    T first, int count,
    int step = 1
  ) : Option<T>(id, name, first, count, step), m_dolphin_info(dolphin_info) { dolphin_options.push_back(this); }
  DolphinOption(
    const char* id,
    const char* name,
    Config::Info<T> dolphin_info,
    bool initial
  ) : Option<T>(id, name, initial), m_dolphin_info(dolphin_info) { dolphin_options.push_back(this); }

  virtual void UpdateDolphin(bool update = true) override
  {
    if (update)
      this->Updated();
    Config::SetCurrent(m_dolphin_info, this->m_value);
  }

  virtual void CheckAndUpdateDolphin() override
  {
    if (this->Updated())
    {
      printf("[DolphinLibretro] Option %s changed\n", this->m_id);
      UpdateDolphin(false);
    }

  }
protected:
  Config::Info<T> m_dolphin_info;
};

// extern DolphinOption<int> internal_resolution;
extern DolphinOption<bool> widescreen;
extern DolphinOption<bool> widescreen_hack;
extern DolphinOption<ShaderCompilationMode> shader_compilation_mode;
extern DolphinOption<bool> wait_for_shaders;
extern DolphinOption<bool> progressive_scan;
extern DolphinOption<bool> use_pal60;
extern DolphinOption<u32> msaa;
extern DolphinOption<bool> ssaa;
extern DolphinOption<AnisotropicFilteringMode> max_anisotropy;
extern DolphinOption<bool> skip_dupe_frames;
extern DolphinOption<bool> immediate_xfb;
extern DolphinOption<bool> scaled_efb_copy;
extern DolphinOption<TextureFilteringMode> texture_filtering;
extern DolphinOption<bool> store_efb_to_texture;
extern DolphinOption<int> texture_cache_accuracy;
extern DolphinOption<bool> gpu_texture_decoding;
extern DolphinOption<bool> fast_depth_calculation;
extern DolphinOption<bool> enable_bounding_box;
extern DolphinOption<bool> disable_efb_to_vram_copies;
extern DolphinOption<bool> dual_core;
extern DolphinOption<PowerPC::CPUCore> cpu_core;
extern DolphinOption<float> cpu_clock_rate;
extern DolphinOption<float> emulation_speed_limit;
extern DolphinOption<int> language;
extern DolphinOption<bool> fastmem;
extern DolphinOption<bool> fast_disc_speed;
extern DolphinOption<bool> enable_rumble;
extern DolphinOption<bool> continuous_scanning_wiimote;
extern DolphinOption<bool> dsp_hle;
extern DolphinOption<bool> dsp_enable_jit;
extern DolphinOption<bool> cheats_enabled;
extern DolphinOption<bool> osd_enabled;
extern DolphinOption<u32> sensor_bar_position;

} // namespace Options
} // namespace Libretro
