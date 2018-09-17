#include <bitstream/opstream.h>
#include <bitstream/sstream.h>
#include <mpeg-isobase/box/time_to_sample.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box/media_samples_size.h>
#include <mpeg-profiler/box/media_header.h>
#include <mpeg-profiler/box/media_samples_decoding_time.h>


namespace mpeg {
namespace profiler {


using bitstream::SStream;
using bitstream::output::print::fourcc;
using bitstream::output::print::pluralize;


void Media::Samples::DecodingTime::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::TimeToSampleBox>(event);
    box.parsing_control().skip_payload_parsing = true;   // do that explicitly

    _entry_count = box.entry_count;
    entries.reserve(_entry_count);
}

void Media::Samples::DecodingTime::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    for(auto box_entry: isobase::TimeToSampleBox::Parsed::Entries{event, _entry_count}) {
        const Entry entry{box_entry.sample_count, box_entry.sample_delta, total_samples};
        if (entry.count == 0) {
            throw Profiler::Exception(SStream()
                    << "Box('stts').entry[" << entries.size() << "].sample_count==0");
        }
        if (entry.delta == 0) {
            throw Profiler::Exception(SStream()
                    << "Box('stts').entry[" << entries.size() << "].sample_delta==0");
        }
        total_samples += entries.emplace_back(entry);
    }
}

void Media::Samples::DecodingTime::seal() {
    const auto &size = parent.any<Samples::Size>('stsz', 'stz2');
    if (sample_count() != size.sample_count()) {
        throw Profiler::Exception(SStream()
                << "Samples count mismatch DecodingTime('stts')["
                << sample_count() << " " << pluralize("sample", sample_count())
                << "] != Size(" << fourcc(size.type) << ")["
                << size.sample_count() << " " << pluralize("sample", size.sample_count()) << "]");
    }
}

Time Media::Samples::DecodingTime::duration(const Media::Header &mdhd) const {
    return mdhd.time(duration());
}

void Media::Samples::DecodingTime::operator >> (isobase::TimeToSampleBox &box) const {
    FullBox::operator >> (box);

    isobase::TimeToSampleBox::Composed::Entries entries{box, this->entries.size()};
    auto i = 0;
    for(auto box_entry : entries) {
        const auto &entry = this->entries[i++];
        box_entry.sample_count = entry.count;
        box_entry.sample_delta = entry.delta;
    }
}

void Media::Samples::DecodingTime::operator >> (OPrintStream &stream) const {
    const auto &mdhd = *dynamic_cast<Media::Samples &>(parent).mdhd;
    stream.out
        << "Decoding times: ["
        << "duration: " << duration(mdhd) << ", "
        << sample_count() << " " << pluralize("sample", sample_count()) << " (in "
        << entries.size() << " " << pluralize("entry", entries.size()) <<")]";
}


}} // namespace mpeg::profiler

