#ifndef __MPEG_PROFILER_BOX_MEDIA_HANDLER__
#define __MPEG_PROFILER_BOX_MEDIA_HANDLER__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/media.h>


namespace mpeg {
namespace isobase {

struct HandlerBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Media::Handler : FullBox { // hdlr

    Handler(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    void operator >> (isobase::HandlerBox &) const;
    virtual void operator >> (OPrintStream &) const;

    uint32_t handler_type;
    std::string name;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_HANDLER__

