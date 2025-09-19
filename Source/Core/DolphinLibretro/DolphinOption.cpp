#include "DolphinLibretro/DolphinOption.h"
#include "DolphinLibretro/InternalResolutionOption.h"
#include "Core/Config/GraphicsSettings.h"
#include "Core/Config/MainSettings.h"
#include "Core/Config/SYSCONFSettings.h"

namespace Libretro::Options
{
std::vector<OptionUpdater*> dolphin_options;

InternalResolutionOption internal_resolution();
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

DolphinOption<int> language(
  "dolphin_language",
  "Language",
  Config::MAIN_GC_LANGUAGE,
  {
    {"English", (int)DiscIO::Language::English-1},
    {"Japanese", (int)DiscIO::Language::Japanese-1},
    {"German", (int)DiscIO::Language::German-1},
    {"French", (int)DiscIO::Language::French-1},
    {"Spanish", (int)DiscIO::Language::Spanish-1},
    {"Italian", (int)DiscIO::Language::Italian-1},
    {"Dutch", (int)DiscIO::Language::Dutch-1},
    {"Simplified Chinese", (int)DiscIO::Language::SimplifiedChinese-1},
    {"Traditional Chinese", (int)DiscIO::Language::TraditionalChinese-1},
    {"Korean", (int)DiscIO::Language::Korean-1}
  }
);


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
}
