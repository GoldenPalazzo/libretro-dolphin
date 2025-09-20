#include <libretro.h>
#include "Common/Logging/Log.h"
#include "VideoCommon/VideoConfig.h"
#include "Core/Config/GraphicsSettings.h"
#include "Core/Config/MainSettings.h"
#include "Core/Config/SYSCONFSettings.h"

#include "DolphinLibretro/Options.h"
#include "DolphinLibretro/DolphinOption.h"
#include "DolphinLibretro/InternalResolutionOption.h"
#include "DolphinLibretro/LanguageOption.h"

namespace Libretro
{
namespace Options
{
static std::vector<retro_variable> optionsList;
static std::vector<bool*> dirtyPtrList;

template <typename T>
void Option<T>::Register()
{
  if (!m_options.empty())
    return;

  m_options = m_name;
  m_options.push_back(';');
  for (auto& option : m_list)
  {
    if (option.first == m_list.begin()->first)
      m_options += std::string(" ") + option.first;
    else
      m_options += std::string("|") + option.first;
  }
  optionsList.push_back({m_id, m_options.c_str()});
  dirtyPtrList.push_back(&m_dirty);
  Updated();
  m_dirty = true;
}

void SetVariables()
{
  if (optionsList.empty())
    return;
  if (optionsList.back().key)
    optionsList.push_back({});
  environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)optionsList.data());
}

void CheckVariables()
{
  bool updated = false;
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && !updated)
    return;

  for (bool* ptr : dirtyPtrList)
    *ptr = true;
}

template <typename T>
Option<T>::Option(const char* id, const char* name,
                  std::initializer_list<std::pair<const char*, T>> list)
    : m_id(id), m_name(name), m_list(list.begin(), list.end())
{
  Register();
}

template <typename T>
Option<T>::Option(const char* id, const char* name, std::initializer_list<const char*> list)
    : m_id(id), m_name(name)
{
  for (auto option : list)
    m_list.push_back({option, (T)m_list.size()});
  Register();
}
template <>
Option<std::string>::Option(const char* id, const char* name,
                            std::initializer_list<const char*> list)
    : m_id(id), m_name(name)
{
  for (auto option : list)
    m_list.push_back({option, option});
  Register();
}
template <>
Option<const char*>::Option(const char* id, const char* name,
                            std::initializer_list<const char*> list)
    : m_id(id), m_name(name)
{
  for (auto option : list)
    m_list.push_back({option, option});
  Register();
}

template <typename T>
Option<T>::Option(const char* id, const char* name, T first,
                  std::initializer_list<const char*> list)
    : m_id(id), m_name(name)
{
  for (auto option : list)
    m_list.push_back({option, first + (int)m_list.size()});
  Register();
}

template <typename T>
Option<T>::Option(const char* id, const char* name, T first, int count, int step)
    : m_id(id), m_name(name)
{
  for (T i = first; i < first + count; i += step)
    m_list.push_back({std::to_string(i), i});
  Register();
}

template <>
Option<bool>::Option(const char* id, const char* name, bool initial) : m_id(id), m_name(name)
{
  m_list.push_back({initial ? "enabled" : "disabled", initial});
  m_list.push_back({!initial ? "enabled" : "disabled", !initial});
  Register();
}

Option<std::string> backend("dolphin_renderer", "Backend", {"Hardware"
#if defined(_DEBUG) || defined(DEBUGFAST)
    , "Software", "Null"
#endif
});
//Option<bool> loadCustomTextures("dolphin_load_custom_textures", "Load Custom Textures", false);
//Option<bool> cacheCustomTextures("dolphin_cache_custom_textures", "Prefetch Custom Textures", false);
Option<int> irMode("dolphin_ir_mode", "Wiimote IR Mode", 1,
    {"Right Stick controls pointer (relative)",
     "Right Stick controls pointer (absolute)",
     "Mouse controls pointer"});
Option<int> irCenter("dolphin_ir_offset", "Wiimote IR Vertical Offset",
    {{"10", 10}, {"11", 11}, {"12", 12}, {"13", 13}, {"14", 14}, {"15", 15}, {"16", 16}, {"17", 17}, {"18", 18}, {"19", 19},
     {"20", 20}, {"21", 21}, {"22", 22}, {"23", 23}, {"24", 24}, {"25", 25}, {"26", 26}, {"27", 27}, {"28", 28}, {"29", 29},
     {"30", 30}, {"31", 31}, {"32", 32}, {"33", 33}, {"34", 34}, {"35", 35}, {"36", 36}, {"37", 37}, {"38", 38}, {"39", 39},
     {"40", 40}, {"41", 41}, {"42", 42}, {"43", 43}, {"44", 44}, {"45", 45}, {"46", 46}, {"47", 47}, {"48", 48}, {"49", 49},
     {"50", 50}, {"-50", -50}, {"-49", -49}, {"-48", -48}, {"-47", -47}, {"-46", -46}, {"-45", -45}, {"-44", -44}, {"-43", -43},
     {"-42", -42}, {"-41", -41}, {"-40", -40}, {"-39", -39}, {"-38", -38}, {"-37", -37}, {"-36", -36}, {"-35", -35}, {"-34", -34},
     {"-33", -33}, {"-32", -32}, {"-31", -31}, {"-30", -30}, {"-29", -29}, {"-28", -28}, {"-27", -27}, {"-26", -26}, {"-25", -25},
     {"-24", -24}, {"-23", -23}, {"-22", -22}, {"-21", -21}, {"-20", -20}, {"-19", -19}, {"-18", -18}, {"-17", -17}, {"-16", -16},
     {"-15", -15}, {"-14", -14}, {"-13", -13}, {"-12", -12}, {"-11", -11}, {"-10", -10}, {"-9", -9}, {"-8", -8}, {"-7", -7},
     {"-6", -6}, {"-5", -5}, {"-4", -4}, {"-3", -3}, {"-2", -2}, {"-1", -1}, {"0", 0}, {"1", 1}, {"2", 2}, {"3", 3}, {"4", 4},
     {"5", 5}, {"6", 6}, {"7", 7}, {"8", 8}, {"9", 9}});
Option<int> irWidth("dolphin_ir_yaw", "Wiimote IR Total Yaw",
    {{"15", 15}, {"16", 16}, {"17", 17}, {"18", 18}, {"19", 19}, {"20", 20}, {"21", 21}, {"22", 22}, {"23", 23}, {"24", 24},
     {"25", 25}, {"26", 26}, {"27", 27}, {"28", 28}, {"29", 29}, {"30", 30}, {"31", 31}, {"32", 32}, {"33", 33}, {"34", 34},
     {"35", 35}, {"36", 36}, {"37", 37}, {"38", 38}, {"39", 39}, {"40", 40}, {"41", 41}, {"42", 42}, {"43", 43}, {"44", 44},
     {"45", 45}, {"46", 46}, {"47", 47}, {"48", 48}, {"49", 49}, {"50", 50}, {"51", 51}, {"52", 52}, {"53", 53}, {"54", 54},
     {"55", 55}, {"56", 56}, {"57", 57}, {"58", 58}, {"59", 59}, {"60", 60}, {"61", 61}, {"62", 62}, {"63", 63}, {"64", 64},
     {"65", 65}, {"66", 66}, {"67", 67}, {"68", 68}, {"69", 69}, {"70", 70}, {"71", 71}, {"72", 72}, {"73", 73}, {"74", 74},
     {"75", 75}, {"76", 76}, {"77", 77}, {"78", 78}, {"79", 79}, {"80", 80}, {"81", 81}, {"82", 82}, {"83", 83}, {"84", 84},
     {"85", 85}, {"86", 86}, {"87", 87}, {"88", 88}, {"89", 89}, {"90", 90}, {"91", 91}, {"92", 92}, {"93", 93}, {"94", 94},
     {"95", 95}, {"96", 96}, {"97", 97}, {"98", 98}, {"99", 99}, {"100", 100}, {"0", 0}, {"1", 1}, {"2", 2}, {"3", 3},
     {"4", 4}, {"5", 5}, {"6", 6}, {"7", 7}, {"8", 8}, {"9", 9}, {"10", 10}, {"11", 11}, {"12", 12}, {"13", 13}, {"14", 14}});
Option<int> irHeight("dolphin_ir_pitch", "Wiimote IR Total Pitch",
    {{"15", 15}, {"16", 16}, {"17", 17}, {"18", 18}, {"19", 19}, {"20", 20}, {"21", 21}, {"22", 22}, {"23", 23}, {"24", 24},
     {"25", 25}, {"26", 26}, {"27", 27}, {"28", 28}, {"29", 29}, {"30", 30}, {"31", 31}, {"32", 32}, {"33", 33}, {"34", 34},
     {"35", 35}, {"36", 36}, {"37", 37}, {"38", 38}, {"39", 39}, {"40", 40}, {"41", 41}, {"42", 42}, {"43", 43}, {"44", 44},
     {"45", 45}, {"46", 46}, {"47", 47}, {"48", 48}, {"49", 49}, {"50", 50}, {"51", 51}, {"52", 52}, {"53", 53}, {"54", 54},
     {"55", 55}, {"56", 56}, {"57", 57}, {"58", 58}, {"59", 59}, {"60", 60}, {"61", 61}, {"62", 62}, {"63", 63}, {"64", 64},
     {"65", 65}, {"66", 66}, {"67", 67}, {"68", 68}, {"69", 69}, {"70", 70}, {"71", 71}, {"72", 72}, {"73", 73}, {"74", 74},
     {"75", 75}, {"76", 76}, {"77", 77}, {"78", 78}, {"79", 79}, {"80", 80}, {"81", 81}, {"82", 82}, {"83", 83}, {"84", 84},
     {"85", 85}, {"86", 86}, {"87", 87}, {"88", 88}, {"89", 89}, {"90", 90}, {"91", 91}, {"92", 92}, {"93", 93}, {"94", 94},
     {"95", 95}, {"96", 96}, {"97", 97}, {"98", 98}, {"99", 99}, {"100", 100}, {"0", 0}, {"1", 1}, {"2", 2}, {"3", 3},
     {"4", 4}, {"5", 5}, {"6", 6}, {"7", 7}, {"8", 8}, {"9", 9}, {"10", 10}, {"11", 11}, {"12", 12}, {"13", 13}, {"14", 14}});
Option<bool> altGCPorts("dolphin_alt_gc_ports_on_wii", "Use ports 5-8 for GameCube controllers in Wii mode", false);
Option<unsigned int> audioMixerRate("dolphin_mixer_rate", "Audio Mixer Rate",
                                    {{"32000", 32000u}, {"48000", 48000u}});
Option<Common::Log::LogLevel> logLevel("dolphin_log_level", "Log Level", {
                                         {"Info", Common::Log::LogLevel::LINFO},
#if defined(_DEBUG) || defined(DEBUGFAST)
                                         {"Debug", Common::Log::LogLevel::LDEBUG},
#endif
                                         {"Notice", Common::Log::LogLevel::LNOTICE},
                                         {"Error", Common::Log::LogLevel::LERROR},
                                         {"Warning", Common::Log::LogLevel::LWARNING}});


std::vector<OptionUpdater*> dolphin_options;

InternalResolutionOption internal_resolution;
DolphinOption<bool> widescreen(
  "dolphin_widescreen",
  "Enable Widescreen (Wii)",
  Config::SYSCONF_WIDESCREEN,
  true
);
DolphinOption<bool> widescreen_hack(
  "dolphin_widescreen_hack",
  "Widescreen Hack",
  Config::GFX_WIDESCREEN_HACK,
  false
);
DolphinOption<ShaderCompilationMode> shader_compilation_mode(
  "dolphin_shader_compilation_mode",
  "Shader Compilation Mode",
  Config::GFX_SHADER_COMPILATION_MODE,
  {
    {"Synchronous", ShaderCompilationMode::Synchronous},
    {"Asynchronous Skip Rendering", ShaderCompilationMode::AsynchronousSkipRendering},
    {"Synchronous Ubershaders", ShaderCompilationMode::SynchronousUberShaders},
    {"Asynchronous Ubershaders", ShaderCompilationMode::AsynchronousUberShaders}
  }
);
DolphinOption<bool> wait_for_shaders(
  "dolphin_wait_for_shaders",
  "Compile Shaders Before Starting",
  Config::GFX_WAIT_FOR_SHADERS_BEFORE_STARTING,
  false
);
DolphinOption<bool> progressive_scan(
  "dolphin_progressive_scan",
  "Progressive Scan",
  Config::SYSCONF_PROGRESSIVE_SCAN,
  true
);
DolphinOption<bool> use_pal60(
  "dolphin_pal60",
  "PAL60",
  Config::SYSCONF_PAL60,
  true
);
DolphinOption<u32> msaa(
  "dolphin_msaa",
  "Anti-Aliasing",
  Config::GFX_MSAA,
  {
    "None",
    "2x MSAA",
    "4x MSAA",
    "8x MSAA",
    "16x MSAA",
    "32x MSAA",
  }
);
DolphinOption<bool> ssaa(
  "dolphin_ssaa",
  "Enable Super Sample Anti-Aliasing (SSAA)",
  Config::GFX_SSAA,
  false
);
DolphinOption<AnisotropicFilteringMode> max_anisotropy(
  "dolphin_max_anisotropy",
  "Max Anisotropy",
  Config::GFX_ENHANCE_MAX_ANISOTROPY,
  {
    "1x Anisotropic",
    "2x Anisotropic",
    "4x Anisotropic",
    "8x Anisotropic",
    "16x Anisotropic",
  }
);
DolphinOption<bool> skip_dupe_frames(
  "dolphin_skip_dupe_frames",
  "Skip Presenting Duplicate Frames",
  Config::GFX_HACK_SKIP_DUPLICATE_XFBS,
  true
);
DolphinOption<bool> immediate_xfb(
  "dolphin_immediate_xfb",
  "Immediately Present XFB",
  Config::GFX_HACK_IMMEDIATE_XFB,
  false
);
DolphinOption<bool> scaled_efb_copy(
  "dolphin_efb_scaled_copy",
  "Scaled EFB Copy",
  Config::GFX_HACK_COPY_EFB_SCALED,
  true
);
DolphinOption<TextureFilteringMode> texture_filtering(
  "dolphin_force_texture_filtering",
  "Force Texture Filtering",
  Config::GFX_ENHANCE_FORCE_TEXTURE_FILTERING,
  {
    "Default",
    "Nearest",
    "Linear"
  }
);
DolphinOption<bool> store_efb_to_texture(
  "dolphin_efb_to_texture",
  "Store EFB Copies to Texture Only",
  Config::GFX_HACK_SKIP_EFB_COPY_TO_RAM,
  true
);
DolphinOption<int> texture_cache_accuracy(
  "dolphin_texture_cache_accuracy",
  "Texture Cache Accuracy",
  Config::GFX_SAFE_TEXTURE_CACHE_COLOR_SAMPLES,
  {
    {"Fast", 128},
    {"Middle", 512},
    {"Safe", 0}
  }
);
DolphinOption<bool> gpu_texture_decoding(
  "dolphin_gpu_texture_decoding",
  "GPU Texture Decoding",
  Config::GFX_ENABLE_GPU_TEXTURE_DECODING,
  false
);
DolphinOption<bool> fast_depth_calculation(
  "dolphin_fast_depth_calculation",
  "Fast Depth Calculation",
  Config::GFX_FAST_DEPTH_CALC,
  true
);
DolphinOption<bool> enable_bounding_box(
  "dolphin_bbox_enabled",
  "Enabled Bounding Box",
  Config::GFX_HACK_BBOX_ENABLE,
  false
);
DolphinOption<bool> disable_efb_to_vram_copies(
  "dolphin_efb_to_vram",
  "Disable EFB to VRAM Copies",
  Config::GFX_HACK_DISABLE_COPY_TO_VRAM,
  false
);
DolphinOption<PowerPC::CPUCore> cpu_core(
  "dolphin_cpu_core",
  "CPU Core",
  Config::MAIN_CPU_CORE,
  {
#ifdef _M_X86_64
    {"JIT64", PowerPC::CPUCore::JIT64},
#elif _M_ARM_64
    {"JITARM64", PowerPC::CPUCore::JITARM64},
#endif
    {"Cached Interpreter", PowerPC::CPUCore::CachedInterpreter},
    {"Interpreter", PowerPC::CPUCore::Interpreter}
  }
);
DolphinOption<float> cpu_clock_rate(
  "dolphin_cpu_clock_rate",
  "CPU Clock Rate",
  Config::MAIN_OVERCLOCK,
  {
    {"100%", 1.0},
    {"150%", 1.5},
    {"200%", 2.0},
    {"250%", 2.5},
    {"300%", 3.0},
    {"350%", 3.5},
    {"400%", 4.0},
    {"5%", 0.05},
    {"10%", 0.1},
    {"20%", 0.2},
    {"30%", 0.3},
    {"40%", 0.4},
    {"50%", 0.5},
    {"60%", 0.6},
    {"70%", 0.7},
    {"80%", 0.8},
    {"90%", 0.9}
  }
);
DolphinOption<float> emulation_speed_limit(
  "dolphin_emulation_speed",
  "Emulation Speed",
  Config::MAIN_EMULATION_SPEED,
  {
    {"unlimited", 0.0},
    {"100%", 1.0}
  }
);

// DolphinOption<int> language(
//   "dolphin_language",
//   "Language",
//   Config::MAIN_GC_LANGUAGE,
//   {
//     {"English", (int)DiscIO::Language::English-1},
//     {"Japanese", (int)DiscIO::Language::Japanese-1},
//     {"German", (int)DiscIO::Language::German-1},
//     {"French", (int)DiscIO::Language::French-1},
//     {"Spanish", (int)DiscIO::Language::Spanish-1},
//     {"Italian", (int)DiscIO::Language::Italian-1},
//     {"Dutch", (int)DiscIO::Language::Dutch-1},
//     {"Simplified Chinese", (int)DiscIO::Language::SimplifiedChinese-1},
//     {"Traditional Chinese", (int)DiscIO::Language::TraditionalChinese-1},
//     {"Korean", (int)DiscIO::Language::Korean-1}
//   }
// );
//
LanguageOption language;

DolphinOption<bool> fastmem(
  "dolphin_fastmem",
  "Fastmem",
  Config::MAIN_FASTMEM,
  true
);
DolphinOption<bool> fast_disc_speed(
  "dolphin_fast_disc_speed",
  "Speed Up Disc Transfer Rate",
  Config::MAIN_FAST_DISC_SPEED,
  false
);
DolphinOption<bool> enable_rumble(
  "dolphin_enable_rumble",
  "Rumble",
  Config::SYSCONF_WIIMOTE_MOTOR,
  true
);
DolphinOption<u32> sensor_bar_position(
  "dolphin_sensor_bar_position",
  "Sensor Bar Position",
  Config::SYSCONF_SENSOR_BAR_POSITION,
  {"Bottom", "Top"}
);
DolphinOption<bool> continuous_scanning_wiimote(
  "dolphin_wiimote_continuous_scanning",
  "Wiimote Continuous Scanning",
  Config::MAIN_WIIMOTE_CONTINUOUS_SCANNING,
  false
);
DolphinOption<bool> dsp_hle(
  "dolphin_dsp_hle",
  "DSP HLE",
  Config::MAIN_DSP_HLE,
  true
);
DolphinOption<bool> dsp_enable_jit(
  "dolphin_dsp_jit",
  "DSP Enable JIT",
  Config::MAIN_DSP_JIT,
  true
);

DolphinOption<bool> cheats_enabled(
  "dolphin_cheats_enabled",
  "Internal Cheats Enabled",
  Config::MAIN_ENABLE_CHEATS,
  false
);
DolphinOption<bool> osd_enabled(
  "dolphin_osd_enabled",
  "OSD Enabled",
  Config::MAIN_OSD_MESSAGES,
  true
);

}  // namespace Options
}  // namespace Libretro
