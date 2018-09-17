#include <bitstream/opstream.h>
#include <mpeg-isobase/box/chunk_offset.h>
#include <mpeg-isobase/box/chunk_large_offset.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box/media_samples_size.h>
#include <mpeg-profiler/box/media_samples_chunk_offset.h>


namespace mpeg {
namespace profiler {


using bitstream::SStream;
using bitstream::output::print::fourcc;
using bitstream::output::print::pluralize;


void Media::Samples::ChunkRegularOffset::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::ChunkOffsetBox>(event);
    box.parsing_control().skip_payload_parsing = true;   // do that explicitly

    _entry_count = box.entry_count;
    entries.reserve(_entry_count);
}

void Media::Samples::ChunkRegularOffset::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    for(auto entry: isobase::ChunkOffsetBox::Parsed::Entries{event, _entry_count}) {
        entries.emplace_back(entry.chunk_offset);
    }
}

void Media::Samples::ChunkRegularOffset::operator >> (isobase::ChunkOffsetBox &box) const {
    FullBox::operator >> (box);

    isobase::ChunkOffsetBox::Composed::Entries entries{box, this->entries.size()};
    auto i = 0;
    for(auto entry : entries) {
        entry.chunk_offset = this->entries[i++];
    }
}

void Media::Samples::ChunkRegularOffset::operator >> (OPrintStream &stream) const {
    stream.out << "Chunks (offsets): [" << entries.size() << " " << pluralize("chunk", entries.size()) << "]";
}



void Media::Samples::ChunkLargeOffset::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::ChunkLargeOffsetBox>(event);
    box.parsing_control().skip_payload_parsing = true;   // do that explicitly

    _entry_count = box.entry_count;
    entries.reserve(_entry_count);
}

void Media::Samples::ChunkLargeOffset::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    for(auto entry: isobase::ChunkLargeOffsetBox::Parsed::Entries{event, _entry_count}) {
        entries.emplace_back(entry.chunk_offset);
    }
}

void Media::Samples::ChunkLargeOffset::operator >> (isobase::ChunkLargeOffsetBox &box) const {
    FullBox::operator >> (box);

    isobase::ChunkLargeOffsetBox::Composed::Entries entries{box, this->entries.size()};
    auto i = 0;
    for(auto entry : entries) {
        entry.chunk_offset = this->entries[i++];
    }
}

void Media::Samples::ChunkLargeOffset::operator >> (OPrintStream &stream) const {
    stream.out << "Chunks (large offsets): [" << entries.size() << " " << pluralize("chunk", entries.size()) << "]";
}


}} // namespace mpeg::profiler

