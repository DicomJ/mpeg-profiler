#ifndef __MPEG_PROFILER_BOX_MEDIA_SAMPLES_CHUNK_OFFSET__
#define __MPEG_PROFILER_BOX_MEDIA_SAMPLES_CHUNK_OFFSET__

#include <mpeg-profiler/box/full.h>
#include <mpeg-profiler/box/media_samples.h>


namespace mpeg {
namespace isobase {

struct ChunkOffsetBox;
struct ChunkLargeOffsetBox;

}} // namespace mpeg::isobase


namespace mpeg {
namespace profiler {


struct Media::Samples::ChunkOffset : FullBox {  // stco/co64

    using FullBox::FullBox;

    virtual uint64_t chunk_offset(int32_t index) const= 0;
    virtual uint32_t chunk_count() const= 0;
};


struct Media::Samples::ChunkRegularOffset : Media::Samples::ChunkOffset {  // stco

    ChunkRegularOffset(Container &container, Box &parent): ChunkOffset(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    void operator >> (isobase::ChunkOffsetBox &) const;
    virtual void operator >> (OPrintStream &) const;

    virtual uint64_t chunk_offset(int32_t index) const { return entries[index]; }
    virtual uint32_t chunk_count() const { return entries.size(); }

private:
    uint32_t _entry_count;
    std::vector<uint32_t> entries;
};


struct Media::Samples::ChunkLargeOffset : Media::Samples::ChunkOffset {  // stco

    ChunkLargeOffset(Container &container, Box &parent): ChunkOffset(container, parent) {}

    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    void operator >> (isobase::ChunkLargeOffsetBox &) const;
    virtual void operator >> (OPrintStream &) const;

    virtual uint64_t chunk_offset(int32_t index) const { return entries[index]; }
    virtual uint32_t chunk_count() const { return entries.size(); }

private:
    uint32_t _entry_count;
    std::vector<uint64_t> entries;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_SAMPLES_CHUNK_OFFSET__

