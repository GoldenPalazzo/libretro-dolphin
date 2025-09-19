#include "DolphinLibretro/DolphinOption.h"
#include "Core/Config/GraphicsSettings.h"
#include "DolphinLibretro/Video.h"

namespace Libretro::Options
{
class InternalResolutionOption final : public DolphinOption<int>
{
public:
  InternalResolutionOption() : DolphinOption<int>(
    "dolphin_efb_scale",
    "Internal Resolution",
    Config::GFX_EFB_SCALE,
    1,
    {
      "Native (640 x 528)",
      "2x Native (1280 x 1056) for 720p",
      "3x Native (1920 x 1584) for 1080p",
      "4x Native (2560 x 2112) for 1440p",
      "5x Native (3200 x 2640)",
      "6x Native (3840 x 3168) for 4K",
      "7x Native (4480 x 3696)",
      "8x Native (5120 x 4224) for 5K",
      "9x Native (5760 x 4752)",
      "10x Native (6400 x 5280)",
      "11x Native (7040 x 5808)",
      "12x Native (7680 x 6336) for 8K"
    }
  ) {}

  virtual void UpdateDolphin(bool _ = false) override
  {
    DolphinOption<int>::UpdateDolphin(false);
    if (!m_initialized)
    {
      m_initialized = true;
      return;
    }
    unsigned cmd = RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO;
    if (Libretro::Video::hw_render.context_type == RETRO_HW_CONTEXT_DIRECT3D)
      cmd = RETRO_ENVIRONMENT_SET_GEOMETRY;

    retro_system_av_info info;
    retro_get_system_av_info(&info);
    Libretro::environ_cb(cmd, &info);
  }

private:
  bool m_initialized = false;

};

extern InternalResolutionOption internal_resolution;
}
