#ifndef __MPEG_PROFILER_BOX_TRACK_HEADER__
#define __MPEG_PROFILER_BOX_TRACK_HEADER__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/track.h>

namespace mpeg {
namespace isobase {

struct TrackHeaderBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Track::Header: FullBox { // tkhd

    Header(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    void operator >> (isobase::TrackHeaderBox &) const;
    virtual void operator >> (OPrintStream &) const;

    virtual void seal();
    virtual bool is_sealed() const { return duration.scale != 0; }

    Time time(uint64_t time) const { return Time{time, timescale()}; }
    uint32_t timescale() const { return duration.scale; }

    uint64_t creation_time;
    uint64_t modification_time;
    uint32_t track_ID;
    Time duration = {0, 0};// scale of 0 is invalid
    uint16_t layer;
    uint16_t alternate_group;
    FixedPoint<int16_t> volume;
    FixedPoint<uint32_t> width;
    FixedPoint<uint32_t> height;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_TRACK_HEADER__

