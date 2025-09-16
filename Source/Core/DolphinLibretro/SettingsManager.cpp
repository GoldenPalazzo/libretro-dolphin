#include "DolphinLibretro/SettingsManager.h"
#include "Core/HW/Wiimote.h"
#include "DolphinLibretro/Input.h"
#include "DolphinLibretro/Options.h"

#include "Core/Config/GraphicsSettings.h"
#include "Core/Config/MainSettings.h"
#include "Core/Config/SYSCONFSettings.h"
#include "Core/ConfigManager.h"
#include "DolphinLibretro/Video.h"
#include "VideoCommon/Widescreen.h"

namespace Libretro
{
namespace Options
{
namespace SettingsManager
{

void UpdateAntialiasing()
{
  switch (Libretro::Options::anti_aliasing)
  {
    case 1:  // 2x MSAA
      Config::SetCurrent(Config::GFX_MSAA, 2);
      Config::SetCurrent(Config::GFX_SSAA, false);
      break;
    case 2:  // 4x MSAA
      Config::SetCurrent(Config::GFX_MSAA, 4);
      Config::SetCurrent(Config::GFX_SSAA, false);
      break;
    case 3:  // 8x MSAA
      Config::SetCurrent(Config::GFX_MSAA, 8);
      Config::SetCurrent(Config::GFX_SSAA, false);
      break;
    case 4:  // 16x MSAA
      Config::SetCurrent(Config::GFX_MSAA, 16);
      Config::SetCurrent(Config::GFX_SSAA, false);
      break;
    case 5:  // 32x MSAA
      Config::SetCurrent(Config::GFX_MSAA, 32);
      Config::SetCurrent(Config::GFX_SSAA, false);
      break;
    case 6:  // 2x SSAA
      Config::SetCurrent(Config::GFX_MSAA, 2);
      Config::SetCurrent(Config::GFX_SSAA, true);
      break;
    case 7:  // 4x SSAA
      Config::SetCurrent(Config::GFX_MSAA, 4);
      Config::SetCurrent(Config::GFX_SSAA, true);
      break;
    case 8:  // 8x SSAA
      Config::SetCurrent(Config::GFX_MSAA, 8);
      Config::SetCurrent(Config::GFX_SSAA, true);
      break;
    case 9:  // 16x SSAA
      Config::SetCurrent(Config::GFX_MSAA, 16);
      Config::SetCurrent(Config::GFX_SSAA, true);
      break;
    case 10: // 32x SSAA
      Config::SetCurrent(Config::GFX_MSAA, 32);
      Config::SetCurrent(Config::GFX_SSAA, true);
      break;
    default: // disabled
      Config::SetCurrent(Config::GFX_MSAA, 1);
      Config::SetCurrent(Config::GFX_SSAA, false);
      break;
  }
}

void InitSettings()
{
#if defined(_DEBUG)
  Config::SetCurrent(Config::MAIN_FASTMEM, false);
#else
  Config::SetCurrent(Config::MAIN_FASTMEM, Libretro::Options::fastmem);
#endif
  Config::SetCurrent(Config::MAIN_DSP_HLE, Libretro::Options::DSPHLE);
  Config::SetCurrent(Config::MAIN_DSP_JIT, Libretro::Options::DSPEnableJIT);
  Config::SetCurrent(Config::MAIN_CPU_CORE, Libretro::Options::cpu_core);
  Config::SetCurrent(Config::MAIN_GC_LANGUAGE, (int)(DiscIO::Language)Libretro::Options::language - 1);
  Config::SetCurrent(Config::MAIN_CPU_THREAD, true);
  Config::SetCurrent(Config::MAIN_EMU_THREAD, false);
  SConfig::GetInstance().bBootToPause = true;
  Config::SetCurrent(Config::MAIN_OVERCLOCK, Libretro::Options::cpu_clock_rate);
  Config::SetCurrent(Config::MAIN_OVERCLOCK_ENABLE, Libretro::Options::cpu_clock_rate != 1.0);
  Config::SetCurrent(Config::MAIN_AUDIO_BACKEND, BACKEND_NULLSOUND);
  Config::SetCurrent(Config::MAIN_DUMP_AUDIO, false);
  Config::SetCurrent(Config::MAIN_DPL2_DECODER, false);
  Config::SetCurrent(Config::MAIN_AUDIO_LATENCY, 0);
  Config::SetCurrent(Config::MAIN_AUDIO_FILL_GAPS, false);
  Config::SetCurrent(Config::MAIN_WIIMOTE_CONTINUOUS_SCANNING, Libretro::Options::WiimoteContinuousScanning);
  Config::SetCurrent(Config::MAIN_ENABLE_CHEATS, Libretro::Options::cheatsEnabled);
  Config::SetCurrent(Config::MAIN_OSD_MESSAGES, Libretro::Options::osdEnabled);
  Config::SetCurrent(Config::MAIN_FAST_DISC_SPEED, Libretro::Options::fastDiscSpeed);

  Config::SetBase(Config::SYSCONF_LANGUAGE, (u32)(DiscIO::Language)Libretro::Options::language);
  Config::SetBase(Config::SYSCONF_WIDESCREEN, Libretro::Options::widescreen);
  Config::SetBase(Config::SYSCONF_PROGRESSIVE_SCAN, Libretro::Options::progressive_scan);
  Config::SetBase(Config::SYSCONF_PAL60, Libretro::Options::use_pal60);
  Config::SetBase(Config::SYSCONF_SENSOR_BAR_POSITION, Libretro::Options::sensorBarPosition);
  Config::SetBase(Config::SYSCONF_WIIMOTE_MOTOR, Libretro::Options::enableRumble);

  Config::SetBase(Config::GFX_WIDESCREEN_HACK, Libretro::Options::widescreen_hack);
  Config::SetBase(Config::GFX_EFB_SCALE, Libretro::Options::internal_resolution);
  Config::SetBase(Config::GFX_ASPECT_RATIO, AspectMode::Stretch);
  Config::SetBase(Config::GFX_BACKEND_MULTITHREADING, false);
  Config::SetBase(Config::GFX_SHADER_COMPILATION_MODE, Libretro::Options::shader_compilation_mode);
  Config::SetBase(Config::GFX_ENHANCE_MAX_ANISOTROPY, Libretro::Options::max_anisotropy);
  Config::SetBase(Config::GFX_HACK_SKIP_DUPLICATE_XFBS, Libretro::Options::skip_presenting_duplicate_frames);
  Config::SetBase(Config::GFX_HACK_IMMEDIATE_XFB, Libretro::Options::immediately_present_xfb);
  Config::SetBase(Config::GFX_HACK_COPY_EFB_SCALED, Libretro::Options::scaled_efb_copy);
  Config::SetBase(Config::GFX_HACK_SKIP_EFB_COPY_TO_RAM, Libretro::Options::store_efb_copies_to_texture);
  Config::SetBase(Config::GFX_HACK_DISABLE_COPY_TO_VRAM, Libretro::Options::disable_efb_to_vram_copies);
  Config::SetBase(Config::GFX_FAST_DEPTH_CALC, Libretro::Options::fast_depth_calculation);
  Config::SetBase(Config::GFX_HACK_BBOX_ENABLE, Libretro::Options::enable_bounding_box);
  Config::SetBase(Config::GFX_ENABLE_GPU_TEXTURE_DECODING, Libretro::Options::gpu_texture_decoding);
  Config::SetBase(Config::GFX_WAIT_FOR_SHADERS_BEFORE_STARTING, Libretro::Options::wait_for_shaders);
  Config::SetBase(Config::GFX_ENHANCE_FORCE_TEXTURE_FILTERING, Libretro::Options::texture_filtering);
  //Config::SetBase(Config::GFX_HIRES_TEXTURES, Libretro::Options::loadCustomTextures);
  //Config::SetBase(Config::GFX_CACHE_HIRES_TEXTURES, Libretro::Options::cacheCustomTextures);
  Config::SetBase(Config::GFX_HIRES_TEXTURES, false); //golden: temporary
  Config::SetBase(Config::GFX_CACHE_HIRES_TEXTURES, false); //golden: temporary
  Config::SetBase(Config::GFX_SAFE_TEXTURE_CACHE_COLOR_SAMPLES, Libretro::Options::texture_cache_accuracy);
#if 0
  Config::SetBase(Config::GFX_SHADER_COMPILER_THREADS, 1);
  Config::SetBase(Config::GFX_SHADER_PRECOMPILER_THREADS, 1);
#endif
  UpdateAntialiasing();

}

void PollAndApply()
{
  if (Libretro::Options::internal_resolution.Updated())
  {
    //g_Config.iEFBScale = Libretro::Options::internal_resolution;
    Config::SetCurrent(Config::GFX_EFB_SCALE, Libretro::Options::internal_resolution);

    unsigned cmd = RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO;
    if (Libretro::Video::hw_render.context_type == RETRO_HW_CONTEXT_DIRECT3D)
      cmd = RETRO_ENVIRONMENT_SET_GEOMETRY;

    retro_system_av_info info;
    retro_get_system_av_info(&info);
    Libretro::environ_cb(cmd, &info);
  }

  if (widescreen != (g_widescreen->IsGameWidescreen() || g_Config.bWidescreenHack))
  {
    retro_system_av_info info;
    retro_get_system_av_info(&info);
    Libretro::environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &info);
  }

  if (Libretro::Options::irMode.Updated() || Libretro::Options::irCenter.Updated()
      || Libretro::Options::irWidth.Updated() || Libretro::Options::irHeight.Updated()
      || Libretro::Options::enableRumble.Updated())
  {
    Libretro::Input::ResetControllers();
  }

  if (Libretro::Options::WiimoteContinuousScanning.Updated())
  {
    Config::SetCurrent(Config::MAIN_WIIMOTE_CONTINUOUS_SCANNING,
                       Libretro::Options::WiimoteContinuousScanning);
    WiimoteReal::Initialize(Wiimote::InitializeMode::DO_NOT_WAIT_FOR_WIIMOTES);
  }

  if (Libretro::Options::anti_aliasing.Updated())
  {
    UpdateAntialiasing();
  }


}

}
}
}
