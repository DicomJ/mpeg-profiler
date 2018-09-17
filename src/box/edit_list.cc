#include <bitstream/opstream.h>
#include <mpeg-isobase/box/edit_list.h>
#include <mpeg-profiler/box/edit_list.h>
#include <mpeg-profiler/box/movie_header.h>
#include <mpeg-profiler/box/media_header.h>


namespace mpeg {
namespace profiler {


void EditList::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::EditListBox>(event);
    box.parsing_control().skip_payload_parsing = true;   // do that explicitly

    _entry_count = box.entry_count;
    entries.reserve(_entry_count);
}

void EditList::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    if (version == 1) {
        for(auto entry: isobase::EditListBox::V1::Parsed::Entries{event, _entry_count}) {
            total.duration += entries.emplace_back(
                    Entry{entry.segment_duration, entry.media_time, entry.media_rate}).segment_duration;
        }
    } else {
        for(auto entry: isobase::EditListBox::V0::Parsed::Entries{event, _entry_count}) {
            total.duration += entries.emplace_back(
                    Entry{entry.segment_duration, entry.media_time, entry.media_rate}).segment_duration;
        }
    }
}

void EditList::operator >> (isobase::EditListBox &box) const {
    FullBox::operator >> (box);

    if (version == 1) {
        isobase::EditListBox::V1::Composed::Entries entries{box, this->entries.size()};
        auto i = 0;
        for(auto box_entry : entries) {
            const auto &entry = this->entries[i++];
            box_entry.segment_duration = entry.segment_duration;
            box_entry.media_time = entry.media_time;
            box_entry.media_rate = entry.media_rate;
        }
    } else {
        isobase::EditListBox::V0::Composed::Entries entries{box, this->entries.size()};
        auto i = 0;
        for(auto box_entry : entries) {
            const auto &entry = this->entries[i++];
            box_entry.segment_duration = entry.segment_duration;
            box_entry.media_time = entry.media_time;
            box_entry.media_rate = entry.media_rate;
        }
    }
}

Time EditList::Entry::duration(const Movie::Header &mvhd) const {
    return mvhd.time(segment_duration);
}

Time EditList::Entry::time(const Media::Header &mdhd) const {
    return mdhd.time(media_time);
}

void EditList::Entry::output(OPrintStream &stream, const EditList &edit_list) const {
    auto &mvhd = edit_list.parent.parent.parent.as<Movie::Header>('mvhd');
    auto &mdhd = edit_list.parent.parent['mdia'].as<Media::Header>('mdhd');

    stream.out << "duration: " << duration(mvhd) << ", ";

    stream.out << "time: ";
    if (media_time == -1L) {
        stream.out << "<empty edit>, ";
    } else {
        stream.out << time(mdhd) << ", ";
    }

    stream.out << "rate: " << double(media_rate);
    if (uint32_t(media_rate) == 0) {
        stream.out << " (dwell)";
    }
}

void EditList::operator >> (OPrintStream &stream) const {
    stream.iout(stream.branching) << "Edit List: [";
    if (entries.size() == 1) {
        entries[0].output(stream, *this);
    } else {
        OPrintStream::Indent indent(stream, std::string(4, ' ')); {
            stream.out << std::endl;
            for (const auto &entry: entries) {
                stream.iout() << "[";
                entry.output(stream, *this);
                stream.out << "]," << std::endl;
            }
        }
    }
    stream.out << "]" << std::endl;
}


}} // namespace mpeg::profiler

