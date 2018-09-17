#ifndef __MPEG_PROFILER_BOX_DATA_ENTRY__
#define __MPEG_PROFILER_BOX_DATA_ENTRY__

#include <mpeg-profiler/box/full.h>


namespace mpeg {
namespace profiler {


struct DataEntry: FullBox {
    DataEntry(Container &container, Box &parent): FullBox(container, parent) {}

    std::string location;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_DATA_ENTRY__

