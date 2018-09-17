#ifndef __MPEG_PROFILER_BOX_MEDIA_SAMPLES_COMPOSITION_OFFSET__
#define __MPEG_PROFILER_BOX_MEDIA_SAMPLES_COMPOSITION_OFFSET__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/media_samples.h>


namespace mpeg {
namespace isobase {

struct CompositionOffsetBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Media::Samples::CompositionOffset : FullBox {

    CompositionOffset(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    void operator >> (isobase::CompositionOffsetBox &) const;
    virtual void operator >> (OPrintStream &) const;

    uint32_t sample_count() const { return total_samples; }
    virtual void seal();

    unsigned long at(uint32_t sample_index) const;

    struct Entry {
        uint32_t count;
        uint32_t offset;
        uint32_t sample_index;
    };
    std::vector<Entry> entries;

protected:
    uint32_t _entry_count;
    uint32_t total_samples = 0;  // absolute index
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_SAMPLES_COMPOSITION_OFFSET__

