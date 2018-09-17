#ifndef __MPEG_PROFILER_BOX_VISUAL_SAMPLE_ENTRY__
#define __MPEG_PROFILER_BOX_VISUAL_SAMPLE_ENTRY__

#include <mpeg-profiler/box/sample_entry.h>


namespace mpeg {
namespace isobase {

struct VisualSampleEntry;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct VisualSampleEntry: SampleEntry {

    VisualSampleEntry(Container &container, Box &parent): SampleEntry(container, parent) {}

    void operator >> (isobase::VisualSampleEntry &) const;
    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void operator >> (OPrintStream &) const;


    std::string compressorname;
    uint16_t width;
    uint16_t height;
    FixedPoint<uint32_t> horizresolution;
    FixedPoint<uint32_t> vertresolution;
    uint16_t frame_count;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_VISUAL_SAMPLE_ENTRY__

