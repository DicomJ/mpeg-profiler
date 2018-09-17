#include <bitstream/opstream.h>
#include <mpeg-isobase/box/sample_size.h>
#include <mpeg-isobase/box/compact_sample_size.h>
#include <mpeg-profiler/box/media_samples_size.h>
#include <mpeg-profiler/box/container.h>


namespace mpeg {
namespace profiler {


using bitstream::output::print::pluralize;


uint64_t Media::Samples::Size::sum_sizes(uint32_t sample_index, uint32_t count) const {
    uint32_t end = sample_index + count;
    assert(end <= sample_count());
    uint64_t sum = 0; for (; sample_index < end; ++sample_index) {
        sum += sample_size(sample_index);
    } return sum;
}


uint64_t Media::Samples::RegularSize::sum_sizes(uint32_t sample_index, uint32_t count) const {
    if (_sample_size != 0) {
        assert(sample_index + count <= sample_count());
        return _sample_size * count;
    }
    else {
        return Size::sum_sizes(sample_index, count);
    }
}

void Media::Samples::RegularSize::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::SampleSizeBox>(event);
    box.parsing_control().skip_payload_parsing = true;   // do that explicitly

    _sample_size = box.sample_size;
    _sample_count = box.sample_count;
    if (_sample_size == 0) {
        entries.reserve(_sample_count);
    } else {
        total.size += _sample_count * _sample_size;
    }
}

void Media::Samples::RegularSize::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    for(auto entry: isobase::SampleSizeBox::Parsed::Entries{event, _sample_count}) {
        uint32_t entry_size = entry.entry_size;
        entries.push_back(entry_size);
        total.size += entry_size;
    }
}

void Media::Samples::RegularSize::operator >> (isobase::SampleSizeBox &box) const {
    FullBox::operator >> (box);

    assert(entries.empty() == (_sample_size != 0));
    box.sample_size = _sample_size;
    if (_sample_size == 0) {
        box.sample_count = _sample_count;
    } else {
        box.sample_count = entries.size();
        isobase::SampleSizeBox::Composed::Entries entries{box, this->entries.size()};
        auto i = 0;
        for(auto entry : entries) {
            entry.entry_size = this->entries[i++];
        }
    }
}

void Media::Samples::RegularSize::operator >> (OPrintStream &stream) const {
    stream.out << "Sizes: [" << _sample_count << " " << pluralize("sample", _sample_count);
    if (_sample_size != 0) {
        stream.out << ", sample_size=" << _sample_size;
    }
    if (total.size != 0) {
        const auto &data_size = container.data_size;
        if (data_size != 0) {
            stream.out << ", total size: " << total.size << " bytes (" << 100.0 * total.size / data_size << "%)";
        }
    }
    stream.out << "]";
}


void Media::Samples::CompactSize::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::CompactSampleSizeBox>(event);

    _sample_count = box.sample_count;
    field_size = box.field_size;
    switch (field_size) {
        case 4: entries4.reserve(_sample_count); break;
        case 8: entries8.reserve(_sample_count); break;
        case 16: entries16.reserve(_sample_count); break;
        default: break;
    }
}

void Media::Samples::CompactSize::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    switch (field_size) {
        case 4: for(auto entry: isobase::CompactSampleSizeBox::V4::Parsed::Entries{event, _sample_count / 2 + _sample_count % 2}) {
            uint32_t entry_size = entry.entry_size;
            entries4.push_back(entry_size);
            total.size += entry_size;
        } break;
        case 8: for(auto entry: isobase::CompactSampleSizeBox::V8::Parsed::Entries{event, _sample_count}) {
            uint32_t entry_size = entry.entry_size;
            entries8.push_back(entry_size);
            total.size += entry_size;
        } break;
        case 16: for(auto entry: isobase::CompactSampleSizeBox::V16::Parsed::Entries{event, _sample_count}) {
            uint32_t entry_size = entry.entry_size;
            entries16.push_back(entry_size);
            total.size += entry_size;
        } break;
        default: assert(false);
    }
}

uint32_t Media::Samples::CompactSize::sample_size(int32_t index) const {
    switch (field_size) {
        case 4: { uint8_t word = entries4[index / 2]; return (index % 2 ? word & (uint8_t(-1) << 4) : word >> 4); };  // 0xF0 | 0x0F
        case 8: return entries8[index];
        case 16: return entries16[index];
        default: assert(false); return 0;
    }
}

void Media::Samples::CompactSize::add_sample_size(uint32_t sample_size) {
    switch (field_size) {
        case 4: entries4.push_back(sample_size);
        case 8: entries8.push_back(sample_size);
        case 16: entries16.push_back(sample_size);
        default: assert(false);
    }
}

void Media::Samples::CompactSize::operator >> (isobase::CompactSampleSizeBox &box) const {
    FullBox::operator >> (box);

    box.field_size = field_size;

    switch (field_size) {
        case 4: {
                isobase::CompactSampleSizeBox::V4::Composed::Entries entries{box, this->entries4.size()};
                auto i = 0;
                for(auto entry : entries) { entry.entry_size = this->entries4[i++]; }
            } break;
        case 8: {
                isobase::CompactSampleSizeBox::V8::Composed::Entries entries{box, this->entries8.size()};
                auto i = 0;
                for(auto entry : entries) { entry.entry_size = this->entries8[i++]; }
            } break;
        case 16: {
                isobase::CompactSampleSizeBox::V16::Composed::Entries entries{box, this->entries16.size()};
                auto i = 0;
                for(auto entry : entries) { entry.entry_size = this->entries16[i++]; }
            } break;
        default: assert(false);
    }
}

void Media::Samples::CompactSize::operator >> (OPrintStream &stream) const {
    stream.out << "Compact sizes: [" << _sample_count << " " << pluralize("sample", _sample_count);
}


}} // namespace mpeg::profiler

