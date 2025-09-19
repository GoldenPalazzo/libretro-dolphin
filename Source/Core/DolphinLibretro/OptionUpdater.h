#pragma once
namespace Libretro::Options
{
  class OptionUpdater
  {
  public:
    virtual void UpdateDolphin(bool update = true) = 0;
    virtual void CheckAndUpdateDolphin() = 0;
  };
}
