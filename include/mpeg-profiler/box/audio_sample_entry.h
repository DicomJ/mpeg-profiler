#ifndef __MPEG_PROFILER_BOX_AUDIO_SAMPLE_ENTRY__
#define __MPEG_PROFILER_BOX_AUDIO_SAMPLE_ENTRY__

#include <mpeg-profiler/box/sample_entry.h>


namespace mpeg {
namespace isobase {

struct AudioSampleEntry;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct AudioSampleEntry: SampleEntry {

    AudioSampleEntry(Container &container, Box &parent): SampleEntry(container, parent) {}

    void operator >> (isobase::AudioSampleEntry &) const;
    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void operator >> (OPrintStream &) const;

    uint16_t channelcount;
    uint16_t samplesize;
    FixedPoint<uint32_t> samplerate;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_AUDIO_SAMPLE_ENTRY__

