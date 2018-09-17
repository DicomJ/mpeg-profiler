#ifndef __MPEG_PROFILER_BOX_MOVIE__
#define __MPEG_PROFILER_BOX_MOVIE__

#include <mpeg-profiler/box.h>


namespace mpeg {
namespace profiler {


struct Movie: Box {  // moov

    struct Header;

    Movie(Container &container, Box &parent): Box(container, parent) {}
    virtual void operator >> (OPrintStream &) const;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MOVIE__

