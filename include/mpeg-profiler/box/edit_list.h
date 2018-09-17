#ifndef __MPEG_PROFILER_BOX_EDIT_LIST__
#define __MPEG_PROFILER_BOX_EDIT_LIST__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/movie.h>
#include <mpeg-profiler/box/media.h>


namespace mpeg {
namespace isobase {

struct EditListBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct EditList: FullBox {
    EditList(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    void operator >> (isobase::EditListBox &box) const;
    virtual void operator >> (OPrintStream &) const;

    uint64_t total_duration() const { return total.duration; }

    struct Entry {
        Time duration(const Movie::Header &mvhd) const;
        Time time(const Media::Header &mvhd) const;

        uint64_t segment_duration;  // mvhd.timescale
        uint64_t media_time;        // mdhd.timescale
        FixedPoint<uint32_t> media_rate;
        void output(OPrintStream &, const EditList &) const;
    };
    std::vector<Entry> entries;

protected:
    uint32_t _entry_count;
    struct {
        uint64_t duration = 0;
    } total;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_EDIT_LIST__

