#include <bitstream/opstream.h>
#include <mpeg-isobase/box/composition_offset.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box/media_samples_size.h>
#include <mpeg-profiler/box/media_samples_composition_offset.h>


namespace mpeg {
namespace profiler {


using bitstream::SStream;
using bitstream::output::print::fourcc;
using bitstream::output::print::pluralize;


void Media::Samples::CompositionOffset::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::CompositionOffsetBox>(event);
    box.parsing_control().skip_payload_parsing = true;   // do that explicitly

    _entry_count = box.entry_count;
    entries.reserve(_entry_count);
}

void Media::Samples::CompositionOffset::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    for(auto entry: isobase::CompositionOffsetBox::V0::Parsed::Entries{event, _entry_count}) {
        total_samples += entries.emplace_back(Entry{entry.sample_count, entry.sample_offset, total_samples}).count;
    }
}

void Media::Samples::CompositionOffset::operator >> (isobase::CompositionOffsetBox &box) const {
    FullBox::operator >> (box);

    isobase::CompositionOffsetBox::V0::Composed::Entries entries{box, this->entries.size()};
    auto i = 0;
    for(auto box_entry : entries) {
        const auto &entry = this->entries[i++];
        box_entry.sample_count = entry.count;
        box_entry.sample_offset = entry.offset;
    }
}

void Media::Samples::CompositionOffset::operator >> (OPrintStream &stream) const {
    stream.out << "Composition times: ["
        << total_samples << " " << pluralize("sample", total_samples) << " (in "
        << entries.size() << " " << pluralize("entry", entries.size()) << "]";
}

void Media::Samples::CompositionOffset::seal() {
    const auto &size = parent.any<Samples::Size>('stsz', 'stz2');
    if (sample_count() != size.sample_count()) {
        throw Profiler::Exception(SStream()
                << "Samples count mismatch CompositionOffset('ctts')["
                << sample_count() << " " << pluralize("sample", sample_count())
                << "] != Size(" << fourcc(size.type) << ")["
                << size.sample_count() << " " << pluralize("sample", size.sample_count()) << "]");
    }
}


}} // namespace mpeg::profiler

