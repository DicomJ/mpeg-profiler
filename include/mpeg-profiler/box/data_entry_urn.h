#ifndef __MPEG_PROFILER_BOX_DATA_ENTRY_URN__
#define __MPEG_PROFILER_BOX_DATA_ENTRY_URN__

#include <mpeg-profiler/box/data_entry.h>


namespace mpeg {
namespace isobase {

struct DataEntryUrnBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct DataEntryUrn: DataEntry {
    DataEntryUrn(Container &container, Box &parent): DataEntry(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    void operator >> (isobase::DataEntryUrnBox &) const;
    virtual void operator >> (OPrintStream &) const;

    std::string name;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_DATA_ENTRY_URN__

