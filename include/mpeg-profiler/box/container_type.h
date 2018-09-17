#ifndef __MPEG_PROFILER_BOX_CONTAINER_TYPE__
#define __MPEG_PROFILER_BOX_CONTAINER_TYPE__

#include <mpeg-profiler/box/container.h>


namespace mpeg {
namespace isobase {

struct FileTypeBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Container::Type: Box { // ftyp/styp

    Type(Container &container, Box &parent): Box(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    void operator >> (isobase::FileTypeBox &) const;
    virtual void operator >> (OPrintStream &) const;

    uint32_t major_brand, minor_version;
    std::vector<uint32_t> compatible_brands;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_CONTAINER_TYPE__

