#ifndef __MPEG_PROFILER_BOX_MEDIA_SAMPLES_DECODING_TIME__
#define __MPEG_PROFILER_BOX_MEDIA_SAMPLES_DECODING_TIME__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/media_samples.h>


namespace mpeg {
namespace isobase {

struct TimeToSampleBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Media::Samples::DecodingTime : FullBox {

    DecodingTime(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    void operator >> (isobase::TimeToSampleBox &) const;
    virtual void operator >> (OPrintStream &) const;

    virtual void seal();
    Time duration(const Media::Header &mdhd) const;
    uint64_t duration() const { return total_samples.time; }
    uint32_t sample_count() const { return total_samples.index; }

    uint32_t seek(uint64_t time) const; // returns sample index
    unsigned long at(uint32_t sample_index) const;  // returns entry index

    struct Entry {
        uint32_t count; // samples
        uint32_t delta; // each sample
        struct Samples {
            uint32_t index;
            uint64_t time;
            void operator += (const Entry &entry) {
                index += entry.count;
                time += entry.count * entry.delta;
            }
        } first_sample;

        uint32_t seek(uint64_t time) const;
    };

    std::vector<Entry> entries;

protected:
    uint32_t _entry_count;
    Entry::Samples total_samples = {0, 0};  // absolute index & time
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_SAMPLES_DECODING_TIME__

