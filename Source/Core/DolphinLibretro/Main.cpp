#include <cstdio>
#include <libretro.h>
#include <string>
#include <thread>

#include "AudioCommon/AudioCommon.h"
#include "AudioCommon/Mixer.h"
#include "AudioCommon/SoundStream.h"
#include "Common/ChunkFile.h"
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
#include "Core/System.h" // Added by me
#include "DolphinLibretro/Input.h"
#include "DolphinLibretro/Options.h"
// #include "DolphinLibretro/Video.h" TODO: currently broken vk_mem_alloc.h import
#include "VideoBackends/OGL/OGLTexture.h"
#include "VideoBackends/OGL/OGLGfx.h" // si chiamava OGL/Render.h
#include "VideoCommon/AsyncRequests.h"
#include "VideoCommon/EFBInterface.h"
#include "VideoCommon/Fifo.h"
#include "VideoCommon/TextureConfig.h"
#include "VideoCommon/VideoCommon.h"
#include "VideoCommon/VideoConfig.h"

#ifdef PERF_TEST
static struct retro_perf_callback perf_cb;

#define RETRO_PERFORMANCE_INIT(name)                                          \
retro_perf_tick_t current_ticks;                                              \
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
  Core::System& sys = Core::System::GetInstance();
  SoundStream* stream = sys.GetSoundStream();
  if (stream)
    return stream->GetMixer()->GetSampleRate();
  else if (sys.IsWii())
    return Options::audioMixerRate;
  else if (Options::audioMixerRate == 32000u)
    return 32029;
  else
    return 48043;
}

class Stream final : public SoundStream
{
  public:
    Stream() : SoundStream(GetSampleRate()) {}
    bool SetRunning(bool running) { return running; }
    void Update()
    {
      unsigned int available = 0; // TODO: change this in the future
                                  // I don't know how to get this value
                                  // because AvailableSamples() doesn't exist
                                  // anymore
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
}
} // namespace Libretro

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
  Libretro::Audio::batch_cb = cb;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
  // Not used
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
  enum retro_pixel_format xrgb8888 = RETRO_PIXEL_FORMAT_XRGB8888;
  Libretro::environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &xrgb8888);
}

void retro_deinit(void)
{
#ifdef PERF_TEST
  perf_cb.perf_log();
#endif
}

void retro_get_system_info(struct retro_system_info* info)
{
  info->need_fullpath = true;
  info->valid_extensions = "elf|dol|gcm|iso|tgc|wbfs|ciso|gcz|wad|wia|rvz|m3u";
  info->library_version = "Golden Testing";
  info->library_name = "dolphin-emu";
  info->block_extract = true;
}

void retro_get_system_av_info(struct retro_system_av_info* info)
{
  info->geometry.base_width = EFB_WIDTH * Libretro::Options::efbScale;
  info->geometry.base_height = EFB_HEIGHT * Libretro::Options::efbScale;
  info->geometry.max_width = info->geometry.base_width;
  info->geometry.max_height = info->geometry.base_height;
  // Temporarily halted to help developing other sections beforehand
}





