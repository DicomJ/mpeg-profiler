#ifndef __MPEG_PROFILER_BOX_MEDIA_SAMPLES_DEPENDENCY_TYPE__
#define __MPEG_PROFILER_BOX_MEDIA_SAMPLES_DEPENDENCY_TYPE__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/media_samples.h>


namespace mpeg {
namespace isobase {

struct SampleDependencyTypeBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Media::Samples::DependencyType : FullBox {  // sdtp

    DependencyType(Container &container, Box &parent): FullBox(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    void operator >> (isobase::SampleDependencyTypeBox &) const;
    virtual void operator >> (OPrintStream &) const;
    uint32_t sample_count() const { return _entry_count; }

    struct Entry {
        uint8_t value;

        uint8_t is_leading()     const { return (value >> 6) & 0x3; }
        uint8_t depends_on()     const { return (value >> 4) & 0x3; }
        uint8_t is_depended_on() const { return (value >> 2) & 0x3; }
        uint8_t has_redundancy() const { return (value >> 0) & 0x3; }

        void operator >> (OPrintStream &) const;
    };
    std::vector<Entry> entries;

protected:
    uint32_t _entry_count;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_SAMPLES_DEPENDENCY_TYPE__

