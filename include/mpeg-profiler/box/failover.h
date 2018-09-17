#ifndef __MPEG_PROFILER_BOX_FAILOVER__
#define __MPEG_PROFILER_BOX_FAILOVER__

#include <mpeg-profiler/box.h>


namespace mpeg {
namespace profiler {


struct FailoverBox: Box {  // moov

    FailoverBox(Container &container, Box &parent): Box(container, parent) {}

    virtual void operator >> (OPrintStream &) const;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_FAILOVER__

