#ifndef __MPEG_PROFILER_BOX_MOVIE_HEADER__
#define __MPEG_PROFILER_BOX_MOVIE_HEADER__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/movie.h>

namespace mpeg {
namespace isobase {

struct MovieHeaderBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Movie::Header: FullBox { // mvhd

    Header(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    void operator >> (isobase::MovieHeaderBox &) const;
    virtual void operator >> (OPrintStream &) const;

    Time time(uint64_t time) const { return Time{time, timescale()}; }
    uint32_t timescale() const { return duration.scale; }

    uint64_t creation_time;
    uint64_t modification_time;
    Time duration;
    FixedPoint<int32_t> rate;
    FixedPoint<int16_t> volume;
    uint32_t next_track_ID;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MOVIE_HEADER__

