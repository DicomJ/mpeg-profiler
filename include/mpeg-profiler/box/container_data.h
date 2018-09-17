#ifndef __MPEG_PROFILER_BOX_CONTAINER_DATA__
#define __MPEG_PROFILER_BOX_CONTAINER_DATA__

#include <mpeg-profiler/box/container.h>


namespace mpeg {
namespace profiler {


struct Container::Data: Box {

    Data(Container &container, Box &parent): Box(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void operator >> (OPrintStream &stream) const;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_CONTAINER_DATA__

