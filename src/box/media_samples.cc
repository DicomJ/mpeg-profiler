#include <algorithm>
#include <bitstream/opstream.h>
#include <mpeg-profiler/box/media_samples.h>
#include <mpeg-profiler/box/media_header.h>
#include <mpeg-profiler/box/media_samples_description.h>
#include <mpeg-profiler/box/media_samples_iterator.h>


namespace mpeg {
namespace profiler {



inline uint32_t
Media::Samples::DecodingTime::Entry::seek(uint64_t time) const {
    uint32_t index = (time - first_sample.time) / delta;
    assert(index < count);
    return first_sample.index + index;
}

inline uint32_t
Media::Samples::DecodingTime::seek(uint64_t time) const {
    //const auto index = binary_search(entries, time,
    const auto index = interpolated_search(entries, time, duration(),
            [](const auto &entry) -> const auto & { return entry.first_sample.time; });
    const auto &entry = entries[index];
    return entry.seek(time);
}

inline uint32_t
Media::Samples::Sync::seek(uint32_t sample_index) const {
    assert(sample_index < total_samples);
    if (entries.empty()) {  // Empty table is allowed
        return sample_index;
    } else {
        return entries[
            //binary_search(entries, sample_index)
            interpolated_search(entries, sample_index, total_samples)
        ];
    }
}



Media::Samples::Index
Media::Samples::seek(Time time, bool no_sync) const {
   return seek(time.scaled(mdhd->timescale()).time, no_sync);
}

Media::Samples::Index
Media::Samples::seek(double seconds, bool no_sync) const {
   return seek(uint64_t(seconds * mdhd->timescale()), no_sync);
}

Media::Samples::Index
Media::Samples::seek(uint64_t time, bool no_sync) const {
    if (time < decoding->duration()) {
        auto sample_index = decoding->seek(time);
        assert(sample_index < sample_count);
        if (!no_sync && _sync != nullptr) {
            sample_index = _sync->seek(sample_index);
            assert(sample_index < sample_count);
        }
        return Index(*this, sample_index);
    } else {
        return end();
    }
}

void Media::Samples::operator >> (OPrintStream &stream) const {
    stream.iout(stream.branching) << "Samples [" << std::endl;
    {
        OPrintStream::Indent indent(stream, std::string(4, ' '));
        for (auto type: {'stts', 'ctts', 'stsd', 'stsz', 'stz2', 'stsc', 'stco', 'co64', 'sdtp', 'stss'}) {
            Box *box = get(type);
            if (box != nullptr) {
                stream.iout();
                stream << *box;
                stream.out << "," << std::endl;
            }
        }
    }
    stream.iout() << "]" << std::endl;

    for (auto it = begin(); it != end(); ++it) {
        *it >> stream;
    }
}

void Media::Samples::seal() {
    mdhd = &parent.parent.as<Media::Header>('mdhd');
    _timescale = mdhd->timescale();
    size = &any<Size>('stsz', 'stz2');
    sample_count = size->sample_count();
    description = &as<Description>('stsd');
    decoding = &as<DecodingTime>('stts');
    composition = get<CompositionOffset>('ctts');
    chunks = &any<ChunkOffset>('stco', 'co64');
    to_chunk = &as<ToChunk>('stsc');
    _sync = get<Sync>('stss');
    dependency = get<DependencyType>('sdtp');
}



}} // namespace mpeg::profiler

