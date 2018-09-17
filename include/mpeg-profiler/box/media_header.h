#ifndef __MPEG_PROFILER_BOX_MEDIA_HEADER__
#define __MPEG_PROFILER_BOX_MEDIA_HEADER__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/media.h>


namespace mpeg {
namespace isobase {

struct MediaHeaderBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Media::Header: FullBox { // mdhd

    Header(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    void operator >> (isobase::MediaHeaderBox &) const;
    virtual void operator >> (OPrintStream &) const;

    Time time(uint64_t time) const { return Time{time, timescale()}; }
    uint32_t timescale() const { return duration.scale; }

    uint64_t creation_time;
    uint64_t modification_time;
    Time duration;
    std::vector<uint8_t> language;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_HEADER__

