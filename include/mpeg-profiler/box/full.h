#ifndef __MPEG_PROFILER_BOX_FULL__
#define __MPEG_PROFILER_BOX_FULL__

#include <mpeg-profiler/box.h>

namespace mpeg {
namespace isobase {

struct FullBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct FullBox: Box {  // full


    FullBox(Container &container, Box &parent): Box(container, parent) {}

    void operator >> (isobase::FullBox &) const;
    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void operator >> (OPrintStream &) const;

    uint8_t version;
    uint32_t flags;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_FULL__

