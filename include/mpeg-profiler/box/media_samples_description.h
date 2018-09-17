#ifndef __MPEG_PROFILER_BOX_MEDIA_SAMPLES_DESCRIPTION__
#define __MPEG_PROFILER_BOX_MEDIA_SAMPLES_DESCRIPTION__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/media_samples.h>


namespace mpeg {
namespace isobase {

struct SampleDescriptionBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Media::Samples::Description : FullBox {

    Description(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    void operator >> (isobase::SampleDescriptionBox &) const;
    virtual void operator >> (OPrintStream &) const;

    uint32_t entry_count() const {
        return children.size();
    }

    virtual std::unique_ptr<Box> create_child_box(const isobase::Box &box, Constructor constructor);
    virtual void seal();

    uint32_t _entry_count;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_SAMPLES_DESCRIPTION__

