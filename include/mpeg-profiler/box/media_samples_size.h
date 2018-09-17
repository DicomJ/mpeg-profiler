#ifndef __MPEG_PROFILER_BOX_MEDIA_SAMPLES_SIZE__
#define __MPEG_PROFILER_BOX_MEDIA_SAMPLES_SIZE__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/media_samples.h>


namespace mpeg {
namespace isobase {

struct SampleSizeBox;
struct CompactSampleSizeBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Media::Samples::Size : FullBox {  // stsz/stz2

    using FullBox::FullBox;

    virtual uint32_t sample_size(int32_t index) const= 0;
    virtual uint32_t sample_count() const= 0;
    virtual void add_sample_size(uint32_t sample_size) = 0;
    uint64_t total_size() const { return total.size; }

    virtual uint64_t sum_sizes(uint32_t sample_index, uint32_t count) const;

protected:
    struct {
        uint64_t size = 0;
    } total;
};


struct Media::Samples::RegularSize : Size {  // stsz

    RegularSize(Container &container, Box &parent): Size(container, parent) {}
    RegularSize(Container &container, Box &parent, uint32_t sample_size, uint32_t sample_count): Size(container, parent), _sample_size(sample_size), _sample_count(sample_count) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    void operator >> (isobase::SampleSizeBox &) const;
    virtual void operator >> (OPrintStream &) const;

    virtual uint32_t sample_size(int32_t index) const { return _sample_size != 0 ? _sample_size: entries[index]; }
    virtual uint32_t sample_count() const { return _sample_count; }
    virtual void add_sample_size(uint32_t sample_size) { entries.push_back(sample_size); }
    virtual uint64_t sum_sizes(uint32_t sample_index, uint32_t count) const;

private:
    uint32_t _sample_size = 0;
    uint32_t _sample_count = 0;
    std::vector<uint32_t> entries;
};


struct Media::Samples::CompactSize : Size {  // stz2

    CompactSize(Container &container, Box &parent): Size(container, parent) {}
    CompactSize(Container &container, Box &parent, uint8_t field_size): Size(container, parent), field_size(field_size) {
        assert(field_size == 4 || field_size == 8 || field_size == 16);
    }

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    void operator >> (isobase::CompactSampleSizeBox &) const;
    virtual void operator >> (OPrintStream &) const;

    virtual uint32_t sample_size(int32_t index) const;
    virtual uint32_t sample_count() const { return _sample_count; }
    virtual void add_sample_size(uint32_t sample_size);

private:
    uint8_t field_size;
    uint32_t _sample_count = 0;
    std::vector<uint8_t> entries4;
    std::vector<uint8_t> entries8;
    std::vector<uint16_t> entries16;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_SAMPLES_SIZE__

