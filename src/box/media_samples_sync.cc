#include <bitstream/opstream.h>
#include <mpeg-isobase/box/sync_sample.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box/media_samples_size.h>
#include <mpeg-profiler/box/media_samples_sync.h>


namespace mpeg {
namespace profiler {


using bitstream::SStream;
using bitstream::output::print::fourcc;
using bitstream::output::print::pluralize;


void Media::Samples::Sync::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::SyncSampleBox>(event);
    box.parsing_control().skip_payload_parsing = true;   // do that explicitly

    _entry_count = box.entry_count;
    entries.reserve(_entry_count);
}

void Media::Samples::Sync::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    for(auto entry: isobase::SyncSampleBox::Parsed::Entries{event, _entry_count}) {
        uint32_t sample_index = entry.sample_number - 1;
        if (entries.empty()) {
            if (sample_index != 0) {
                throw Profiler::Exception(SStream()
                        << "Box('stss').entry[0]=" << sample_index + 1 << ", but must point to the fist sample");
            }
        } else {
            auto previous_sample_index = entries.back();
            if (previous_sample_index >= sample_index) {
                throw Profiler::Exception(SStream()
                        << "Sync entries are not strictly inreased entry[" << entries.size() << "]="
                        << previous_sample_index << " suppose to be less then following syn sample=" << sample_index);
            }
        }
        entries.emplace_back(sample_index);
    }
}

void Media::Samples::Sync::operator >> (isobase::SyncSampleBox &box) const {
    FullBox::operator >> (box);

    isobase::SyncSampleBox::Composed::Entries entries{box, this->entries.size()};
    auto i = 0;
    for(auto entry : entries) {
        entry.sample_number = this->entries[i++] + 1;
    }
}

void Media::Samples::Sync::operator >> (OPrintStream &stream) const {
    stream.out << "Sync samples: [" << entries.size() << " " << pluralize("entry", entries.size()) << "]";
}

void Media::Samples::Sync::seal() {
    if (!entries.empty()) {
        if (entries.front() != 0) {
            throw Profiler::Exception(SStream()
                    << "First sync('stss') sample[0]=" << entries.front() << " doesn't point to first sample");
        }

        const auto &size = parent.any<Samples::Size>('stsz', 'stz2');
        total_samples = size.sample_count();
        if (entries.back() >= total_samples) {
            throw Profiler::Exception(SStream()
                    << "Last sync('stss') sample[" << entries.size() << "]" << entries.back()
                    << " exceeds total sample count " << total_samples);
        }
    }
}


}} // namespace mpeg::profiler

