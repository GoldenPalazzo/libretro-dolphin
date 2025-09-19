#pragma once

#include <cassert>
#include <libretro.h>
#include <string>
#include <vector>

#include "Common/Logging/Log.h"
#include "Core/PowerPC/PowerPC.h"
#include "DiscIO/Enums.h"
#include "VideoCommon/VideoConfig.h"

namespace Libretro
{
extern retro_environment_t environ_cb;

namespace Options
{
void SetVariables();
void CheckVariables();
void Register(const char* id, const char* desc, bool* dirtyPtr);

template <typename T>
class Option
{
public:
  Option(const char* id, const char* name, std::initializer_list<std::pair<const char*, T>> list);
  Option(const char* id, const char* name, std::initializer_list<const char*> list);
  Option(const char* id, const char* name, T first, std::initializer_list<const char*> list);
  Option(const char* id, const char* name, T first, int count, int step = 1);
  Option(const char* id, const char* name, bool initial);

  bool Updated() const
  {
    if (m_dirty)
    {
      m_dirty = false;

      retro_variable var{m_id};
      T value = m_list.front().second;

      if (environ_cb && environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
      {
        for (auto option : m_list)
        {
          if (option.first == var.value)
          {
            value = option.second;
            break;
          }
        }
      }

      if (m_value != value)
      {
        m_value = value;
        return true;
      }
    }
    return false;
  }

  operator T() const
  {
    Updated();
    return m_value;
  }

  template <typename S>
  bool operator==(S value)
  {
    return (T)(*this) == value;
  }

  template <typename S>
  bool operator!=(S value)
  {
    return (T)(*this) != value;
  }

protected:
  void Register();

  const char* m_id;
  const char* m_name;
  mutable T m_value;
  mutable bool m_dirty = true;
  std::string m_options;
  std::vector<std::pair<std::string, T>> m_list;
};

//extern Option<std::string> renderer;
//extern Option<int> efbScale;
//extern Option<bool> Widescreen;
//extern Option<bool> WidescreenHack;
//extern Option<ShaderCompilationMode> shaderCompilationMode;
//extern Option<bool> waitForShaders;
//extern Option<bool> progressiveScan;
//extern Option<bool> pal60;
//extern Option<int> antiAliasing;
//extern Option<AnisotropicFilteringMode> maxAnisotropy;
//extern Option<bool> skipDupeFrames;
//extern Option<bool> immediatexfb;
//extern Option<bool> efbScaledCopy;
//extern Option<TextureFilteringMode> forceTextureFiltering;
//extern Option<bool> efbToTexture;
//extern Option<int> textureCacheAccuracy;
//extern Option<bool> gpuTextureDecoding;
//extern Option<bool> fastDepthCalc;
//extern Option<bool> bboxEnabled;
//extern Option<bool> efbToVram;
//extern Option<bool> loadCustomTextures;
//extern Option<bool> cacheCustomTextures;
//extern Option<PowerPC::CPUCore> cpu_core;
//extern Option<float> cpuClockRate;
//extern Option<float> EmulationSpeed;
// extern Option<bool> fastmem;
// extern Option<bool> fastDiscSpeed;
extern Option<int> irMode;
extern Option<int> irCenter;
extern Option<int> irWidth;
extern Option<int> irHeight;
// extern Option<bool> enableRumble;
// extern Option<u32> sensorBarPosition;
// extern Option<bool> WiimoteContinuousScanning;
extern Option<bool> altGCPorts;
extern Option<unsigned int> audioMixerRate;
// extern Option<bool> DSPHLE;
// extern Option<bool> DSPEnableJIT;
// extern Option<bool> cheatsEnabled;
extern Option<bool> osdEnabled;
extern Option<Common::Log::LogLevel> logLevel;

// General options
// extern Option<bool> dual_core;
// extern Option<float> emulation_speed_limit;

// Graphics options general
extern Option<std::string> backend;
//extern Option<int> aspect_ratio;
// extern Option<bool> vsync;
// extern Option<ShaderCompilationMode> shader_compilation_mode;
// extern Option<bool> wait_for_shaders;
// Graphics options enhancements
// extern Option<int> internal_resolution; //old efbScale
// extern Option<int> anti_aliasing;
// extern Option<TextureFilteringMode> texture_filtering;
// extern Option<AnisotropicFilteringMode> max_anisotropy;
// extern Option<OutputResamplingMode> output_resampling;
// extern Option<bool> scaled_efb_copy;
// extern Option<bool> widescreen_hack;
// extern Option<bool> disable_fog;
// extern Option<bool> per_pixel_lighting;
// Graphics options hacks
// extern Option<bool> skip_efb_access_from_cpu;
// extern Option<bool> store_efb_copies_to_texture;
// extern Option<bool> store_xfb_copies_to_texture;
// extern Option<bool> immediately_present_xfb;
// extern Option<bool> skip_presenting_duplicate_frames;
// extern Option<bool> enable_bounding_box;
// extern Option<bool> fast_depth_calculation;
// extern Option<int> texture_cache_accuracy;
// extern Option<bool> gpu_texture_decoding;
// Graphics options advanced
// extern Option<bool> progressive_scan;
// extern Option<bool> disable_efb_to_vram_copies;
// Wii options
// extern Option<bool> use_pal60;
// extern Option<DiscIO::Language> language;
// extern Option<bool> widescreen;

// Advanced options
// extern Option<PowerPC::CPUCore> cpu_core;
// extern Option<float> cpu_clock_rate;

}  // namespace Options
}  // namespace Libretro
