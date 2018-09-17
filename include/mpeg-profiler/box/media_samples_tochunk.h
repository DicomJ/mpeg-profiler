#ifndef __MPEG_PROFILER_BOX_MEDIA_SAMPLES_TOCHUNK__
#define __MPEG_PROFILER_BOX_MEDIA_SAMPLES_TOCHUNK__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/media_samples.h>


namespace mpeg {
namespace isobase {

struct SampleToChunkBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Media::Samples::ToChunk: FullBox {  // stsc

    ToChunk(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    void operator >> (isobase::SampleToChunkBox &) const;
    virtual void operator >> (OPrintStream &) const;

    uint32_t sample_count() const { return total.samples; }
    virtual bool is_sealed() const { return !runs.empty() && runs.back().chunks != 0; }

    unsigned long at(uint32_t sample_index) const;

    struct Run {
        uint32_t first_chunk_index;         // = box.first_chunk - 1
        uint32_t samples_per_chunk;
        uint32_t sample_description_index;  // box.sample_description_index - 1

        uint32_t sample_index = 0;          // sample index
        uint32_t chunks = 0;                // Number of chunks in this run
    };

    std::vector<Run> runs;

protected:
    uint32_t _runs_count;
    struct {
        uint32_t chunks = 0;
        uint32_t samples = 0;
    } total;

    virtual void seal();
    inline void update_last_run_and_totals(uint32_t chunk_count);
};



}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_SAMPLES_TOCHUNK__

