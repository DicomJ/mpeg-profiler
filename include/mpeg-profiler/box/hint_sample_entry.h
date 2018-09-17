#ifndef __MPEG_PROFILER_BOX_HINT_SAMPLE_ENTRY__
#define __MPEG_PROFILER_BOX_HINT_SAMPLE_ENTRY__

#include <mpeg-profiler/box/sample_entry.h>


namespace mpeg {
namespace isobase {

struct HintSampleEntry;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct HintSampleEntry: SampleEntry {

    HintSampleEntry(Container &container, Box &parent): SampleEntry(container, parent) {}

    void operator >> (isobase::HintSampleEntry &) const;
    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void operator >> (OPrintStream &) const;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_HINT_SAMPLE_ENTRY__

