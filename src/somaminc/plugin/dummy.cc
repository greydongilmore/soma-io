// only needed on Mac bundle
#include <soma-io/plugin/mincplugin.h>

using namespace soma;

namespace
{
  // force using libsomaminc.dylib on Mac
  bool __mincplugin = MincPlugin::noop();
}

