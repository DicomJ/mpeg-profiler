#ifndef __MPEG_PROFILER_BOX_MEDIA__
#define __MPEG_PROFILER_BOX_MEDIA__

#include <mpeg-profiler/box.h>


namespace mpeg {
namespace profiler {


struct Media: Box {  // mdia

    struct Header;
    struct Handler;
    struct Sample;
    struct Samples;

    Media(Container &container, Box &parent): Box(container, parent) {}
    virtual void operator >> (OPrintStream &) const;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA__

