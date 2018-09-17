#include <bitstream/opstream.h>
#include <bitstream/sstream.h>
#include <mpeg-isobase/box/sample_dependency_type.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box/media_samples_dependency_type.h>
#include <mpeg-profiler/box/media_samples_size.h>


namespace mpeg {
namespace profiler {


using bitstream::SStream;
using bitstream::output::print::fourcc;
using bitstream::output::print::pluralize;


void Media::Samples::DependencyType::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::SampleDependencyTypeBox>(event);
    box.parsing_control().skip_payload_parsing = true;   // do that explicitly

    _entry_count = box.entries();

    const auto &size = parent.any<Samples::Size>('stsz', 'stz2');
    if (sample_count() != size.sample_count()) {
        throw Profiler::Exception(SStream()
                << "Samples count mismatch DependencyType('sdtp)["
                << sample_count() << " " << pluralize("sample", sample_count())
                << "] != Size(" << fourcc(size.type) << ")["
                << size.sample_count() << " " << pluralize("sample", size.sample_count()) << "]");
    }

    entries.reserve(_entry_count);
}

void Media::Samples::DependencyType::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    for(auto entry: isobase::SampleDependencyTypeBox::Parsed::Entries{event, _entry_count}) {
        entries.emplace_back(Entry{entry.entire_mask});
    }
}

void Media::Samples::DependencyType::operator >> (isobase::SampleDependencyTypeBox &box) const {
    FullBox::operator >> (box);

    isobase::SampleDependencyTypeBox::Composed::Entries entries{box, this->entries.size()};
    auto i = 0;
    for(auto entry : entries) {
        entry.entire_mask = this->entries[i++].value;
    }
}

void Media::Samples::DependencyType::operator >> (OPrintStream &stream) const {
    stream.out << "Dependencies: [" << entries.size() << " " << pluralize("entry", entries.size()) << "]";
}

void Media::Samples::DependencyType::Entry::operator >> (OPrintStream &stream) const {
    //stream.out << "mask: " << uint32_t(value) << ", ";
    stream.out << "is_leading: ";
    switch (is_leading()) {
        case 0: stream.out << "unknown"; break;
        case 1: stream.out << "not decodable"; break;
        case 2: stream.out << "is not a leading"; break;
        case 3: stream.out << "is decodable"; break;
    }
    stream.out << ", depends_on: ";
    switch (depends_on()) {
        case 0: stream.out << "unknown"; break;
        case 1: stream.out << "not an I picture"; break;
        case 2: stream.out << "I picture"; break;
        case 3: stream.out << "reserved"; break;
    }
    stream.out << ", is_depended_on: ";
    switch (is_depended_on()) {
        case 0: stream.out << "unknown"; break;
        case 1: stream.out << "not disposable"; break;
        case 2: stream.out << "disposable"; break;
        case 3: stream.out << "reserved"; break;
    }
    stream.out << ", has_redundancy: ";
    switch (has_redundancy()) {
        case 0: stream.out << "unknown"; break;
        case 1: stream.out << "is redundant coding"; break;
        case 2: stream.out << "is no redundant coding"; break;
        case 3: stream.out << "reserved"; break;
    }
}


}} // namespace mpeg::profiler

