#include <cstdint>
#include <libretro.h>
#include <string>
#include <thread>

#include "AudioCommon/AudioCommon.h"
#include "Common/ChunkFile.h"
#include "Common/Config/Config.h"
#include "Common/Event.h"
#include "Common/GL/GLContext.h"
#include "Common/Logging/LogManager.h"
#include "Common/Thread.h"
#include "Common/Version.h"
#include "Core/BootManager.h"
#include "Core/Config/MainSettings.h"
#include "Core/Config/SYSCONFSettings.h"
#include "Core/ConfigManager.h"
#include "Core/Core.h"
#include "Core/HW/CPU.h"
#include "Core/HW/Memmap.h"
#include "Core/HW/ProcessorInterface.h"
#include "Core/HW/VideoInterface.h"
#include "Core/HW/WiimoteReal/WiimoteReal.h"
#include "Core/State.h"
#include "Core/System.h"
#include "DolphinLibretro/Input.h"
#include "DolphinLibretro/Options.h"
#include "DolphinLibretro/Video.h"
#include "VideoBackends/OGL/OGLTexture.h"
#include "VideoBackends/OGL/OGLGfx.h"
#include "VideoCommon/AsyncRequests.h"
#include "VideoCommon/Fifo.h"
#include "VideoCommon/TextureConfig.h"
#include "VideoCommon/VideoBackendBase.h"
#include "VideoCommon/VideoCommon.h"
#include "VideoCommon/VideoConfig.h"

#ifdef PERF_TEST
static struct retro_perf_callback perf_cb;

#define RETRO_PERFORMANCE_INIT(name)                                          \
  retro_perf_tick_t current_ticks;                                            \
  static struct retro_perf_counter name = {#name};                            \
  if (!name.registered)                                                       \
    perf_cb.perf_register(&(name));                                           \
  current_ticks = name.total

#define RETRO_PERFORMANCE_START(name) perf_cb.perf_start(&(name))
#define RETRO_PERFORMANCE_STOP(name)                                          \
  perf_cb.perf_stop(&(name));                                                 \
  current_ticks = name.total - current_ticks;
#else
#define RETRO_PERFORMANCE_INIT(name)
#define RETRO_PERFORMANCE_START(name)
#define RETRO_PERFORMANCE_STOP(name)
#endif

namespace Libretro
{
retro_environment_t environ_cb;
static bool widescreen;

namespace Audio
{
static retro_audio_sample_batch_t batch_cb;

static unsigned int GetSampleRate()
{
  auto& system = Core::System::GetInstance();
  SoundStream* sound_stream = system.GetSoundStream();
  if (sound_stream)
    return sound_stream->GetMixer()->GetSampleRate();
  else if (system.IsWii())
    return Options::audioMixerRate;
  else if (Options::audioMixerRate == 32000u)
    return 32029;

  return 48043;
}

class Stream final : public SoundStream
{
public:
  Stream() : SoundStream(GetSampleRate()) {}
  bool SetRunning(bool running) override { return running; }
  // TODO: this was an override... fix it later if needed
  void Update() // override
  {
    //unsigned int available = m_mixer->AvailableSamples();
    unsigned int available = 0; // TODO: There is no AvailableSamples anymore, fix later
    while (available > MAX_SAMPLES)
    {
      m_mixer->Mix(m_buffer, MAX_SAMPLES);
      batch_cb(m_buffer, MAX_SAMPLES);
      available -= MAX_SAMPLES;
    }
    if (available)
    {
      m_mixer->Mix(m_buffer, available);
      batch_cb(m_buffer, available);
    }
  }

private:
  static constexpr unsigned int MAX_SAMPLES = 512;
  s16 m_buffer[MAX_SAMPLES * 2];
};

}  // namespace Audio
}  // namespace Libretro

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
  Libretro::Audio::batch_cb = cb;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
}

void retro_set_environment(retro_environment_t cb)
{
  Libretro::environ_cb = cb;
  Libretro::Options::SetVariables();
#ifdef PERF_TEST
  environ_cb(RETRO_ENVIRONMENT_GET_PERF_INTERFACE, &perf_cb);
#endif
}

void retro_init(void)
{
  enum retro_pixel_format xrgb888 = RETRO_PIXEL_FORMAT_XRGB8888;
  Libretro::environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &xrgb888);
}

void retro_deinit(void)
{
#ifdef PERF_TEST
  perf_cb.perf_log();
#endif
}

void retro_get_system_info(retro_system_info* info)
{
  info->need_fullpath = true;
  info->valid_extensions = "elf|dol|gcm|iso|tgc|wbfs|ciso|gcz|wad|wia|rvz|m3u";
  info->library_version = Common::GetScmDescStr().c_str();
  info->library_name = "dolphin-emu";
  info->block_extract = true;
}

void retro_get_system_av_info(retro_system_av_info* info)
{
  info->geometry.base_width  = EFB_WIDTH * Libretro::Options::efbScale;
  info->geometry.base_height = EFB_HEIGHT * Libretro::Options::efbScale;

  info->geometry.max_width   = info->geometry.base_width;
  info->geometry.max_height  = info->geometry.base_height;

  // TODO: IsWideScreen was added as a libretro patch in RenderBase.h
  // but the file doesn't exist anymore.
  // Leaving that for the moment
  //if (g_renderer)
    //Libretro::widescreen = g_renderer->IsWideScreen() || g_Config.bWidescreenHack;
  //else if (SConfig::GetInstance().bWii)
    Libretro::widescreen = Config::Get(Config::SYSCONF_WIDESCREEN);

  info->geometry.aspect_ratio = Libretro::widescreen ? 16.0 / 9.0 : 4.0 / 3.0;
  info->timing.fps = (retro_get_region() == RETRO_REGION_NTSC) ? (60.0f / 1.001f) : 50.0f;
  info->timing.sample_rate = Libretro::Audio::GetSampleRate();
}

void retro_reset(void)
{
  Core::System::GetInstance().GetProcessorInterface().ResetButton_Tap();
}

void retro_run(void)
{
  Libretro::Options::CheckVariables();
#if defined(_DEBUG)
  Common::Log::LogManager::GetInstance()->SetLogLevel(Common::Log::LDEBUG);
#else
  Common::Log::LogManager::GetInstance()->SetLogLevel(Libretro::Options::logLevel);
#endif
  Config::SetCurrent(Config::MAIN_OVERCLOCK, Libretro::Options::cpuClockRate);
  Config::SetCurrent(Config::MAIN_OVERCLOCK_ENABLE, Libretro::Options::cpuClockRate != 1.0);
  g_Config.bWidescreenHack = Libretro::Options::WidescreenHack;

  Libretro::Input::Update();

  auto& system = Core::System::GetInstance();
  if (Core::GetState(system) == Core::State::Starting)
  {
    WindowSystemInfo wsi(WindowSystemType::Libretro, nullptr, nullptr, nullptr);
    Core::EmuThread(system, wsi);
    AudioCommon::SetSoundStreamRunning(system, false);
    system.SetSoundStream(std::make_unique<Libretro::Audio::Stream>());
    AudioCommon::SetSoundStreamRunning(system, true);

    // TODO: Software renderer not working at the moment
    // Check Source/Core/DolphinLibretro/Video.h
    //if (Config::Get(Config::MAIN_GFX_BACKEND) == "Software Renderer")
    //{
      //g_video_backend->ShutdownShared();
      //g_renderer.reset();
      //g_renderer = std::make_unique<Libretro::Video::SWRenderer>();
    //}
    //else
    if (Config::Get(Config::MAIN_GFX_BACKEND) == "Null")
    {
      g_gfx.reset();
      //g_renderer.reset();
      g_gfx = std::make_unique<Libretro::Video::NullRenderer>();
    }

    while (!Core::IsRunning(system))
      Common::SleepCurrentThread(100);
  }

  if (Config::Get(Config::MAIN_GFX_BACKEND) == "OGL")
  {
    static_cast<OGL::OGLGfx*>(g_gfx.get())
        ->SetSystemFrameBuffer((GLuint)Libretro::Video::hw_render.get_current_framebuffer());
  }

  if (Libretro::Options::efbScale.Updated())
  {
    g_Config.iEFBScale = Libretro::Options::efbScale;

    unsigned cmd = RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO;
    if (Libretro::Video::hw_render.context_type == RETRO_HW_CONTEXT_DIRECT3D)
      cmd = RETRO_ENVIRONMENT_SET_GEOMETRY;

    retro_system_av_info info;
    retro_get_system_av_info(&info);
    Libretro::environ_cb(cmd, &info);
  }

  if (Libretro::widescreen != (/*g_renderer->IsWideScreen()*/ false || g_Config.bWidescreenHack))
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

  RETRO_PERFORMANCE_INIT(dolphin_main_func);
  RETRO_PERFORMANCE_START(dolphin_main_func);

  AsyncRequests::GetInstance()->SetEnable(true);
  AsyncRequests::GetInstance()->SetPassthrough(false);
  Core::DoFrameStep(system);
  system.GetFifo().RunGpuLoop();
  if (!system.GetFifo().UseDeterministicGPUThread())
  {
    AsyncRequests::GetInstance()->SetEnable(false);
    AsyncRequests::GetInstance()->SetPassthrough(true);
  }

  RETRO_PERFORMANCE_STOP(dolphin_main_func);
}

// TODO: disable savestates for the moment
size_t retro_serialize_size(void)
{
#if 0
  size_t size = 0;

  const Core::CPUThreadGuard guard(Core::System::GetInstance());
  PointerWrap p((u8**)&size, PointerWrap::MODE_MEASURE);
  State::DoState(p);

  return size;
#else
  return 0;
#endif
}

bool retro_serialize(void* data, size_t size)
{
#if 0
  Core::RunAsCPUThread([&] {
    PointerWrap p((u8**)&data, PointerWrap::MODE_WRITE);
    State::DoState(p);
  });

  return true;
#else
  return false;
#endif
}
bool retro_unserialize(const void* data, size_t size)
{
#if 0
  Core::RunAsCPUThread([&] {
    PointerWrap p((u8**)&data, PointerWrap::MODE_READ);
    State::DoState(p);
  });

  return true;
#else
  return false;
#endif
}

unsigned retro_get_region(void)
{
  if (DiscIO::IsNTSC(SConfig::GetInstance().m_region) ||
      (Core::System::GetInstance().IsWii() && Config::Get(Config::SYSCONF_PAL60)))
    return RETRO_REGION_NTSC;

  return RETRO_REGION_PAL;
}

unsigned retro_api_version()
{
  return RETRO_API_VERSION;
}

size_t retro_get_memory_size(unsigned id)
{
  if (id == RETRO_MEMORY_SYSTEM_RAM)
  {
    auto& system = Core::System::GetInstance();
    return system.GetMemory().GetTotalMemorySize();
  }
  return 0;
}

void* retro_get_memory_data(unsigned id)
{
  if (id == RETRO_MEMORY_SYSTEM_RAM)
  {
    auto &system = Core::System::GetInstance();
    return system.GetMemory().GetContiguousRAM();
  }
  return NULL;
}

void retro_cheat_reset(void)
{
}

void retro_cheat_set(unsigned index, bool enabled, const char* code)
{
}
