#ifndef __MPEG_PROFILER_BOX_DATA_ENTRY_URL__
#define __MPEG_PROFILER_BOX_DATA_ENTRY_URL__

#include <mpeg-profiler/box/data_entry.h>


namespace mpeg {
namespace isobase {

struct DataEntryUrlBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct DataEntryUrl: DataEntry {
    DataEntryUrl(Container &container, Box &parent): DataEntry(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    void operator >> (isobase::DataEntryUrlBox &) const;
    virtual void operator >> (OPrintStream &) const;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_DATA_ENTRY_URL__

