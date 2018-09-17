#ifndef __MPEG_PROFILER_BOX_SAMPLE_ENTRY__
#define __MPEG_PROFILER_BOX_SAMPLE_ENTRY__

#include <mpeg-profiler/box.h>


namespace mpeg {
namespace isobase {

struct SampleEntry;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct DataEntry;


struct SampleEntry: Box {

    SampleEntry(Container &container, Box &parent): Box(container, parent) {}

    void operator >> (isobase::SampleEntry &) const;
    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void operator >> (OPrintStream &) const;

    virtual void seal();
    virtual bool is_sealed() const { return _data_entry != nullptr; }

    uint16_t data_reference_index;
    const DataEntry *data_entry() const { return _data_entry; }
    const DataEntry *_data_entry = nullptr;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_SAMPLE_ENTRY__

