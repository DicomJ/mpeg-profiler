#ifndef __MPEG_PROFILER_BOX_MEDIA_SAMPLES_SYNC__
#define __MPEG_PROFILER_BOX_MEDIA_SAMPLES_SYNC__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/media_samples.h>


namespace mpeg {
namespace isobase {

struct SyncSampleBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Media::Samples::Sync : FullBox {  // stss

    Sync(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    void operator >> (isobase::SyncSampleBox &) const;
    virtual void operator >> (OPrintStream &) const;
    virtual void seal();

    uint32_t seek(uint32_t sample_index) const; // returns sample index

    std::vector<uint32_t> entries;

protected:
    uint32_t _entry_count;
    uint32_t total_samples = 0;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_SAMPLES_SYNC__

