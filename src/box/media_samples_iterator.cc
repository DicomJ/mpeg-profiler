#include <algorithm>
#include <bitstream/opstream.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box/media_samples_size.h>
#include <mpeg-profiler/box/media_samples_description.h>
#include <mpeg-profiler/box/media_samples_iterator.h>

namespace mpeg {
namespace profiler {


using bitstream::SStream;
using bitstream::output::print::pluralize;


unsigned long
Media::Samples::DecodingTime::at(uint32_t sample_index) const {
    assert(sample_index < sample_count());
    //return binary_search(entries, sample_index,
    return interpolated_search(entries, sample_index, sample_count(),
            [](const auto &entry) -> const auto & { return entry.first_sample.index; });

}

unsigned long
Media::Samples::CompositionOffset::at(uint32_t sample_index) const {
    assert(sample_index < sample_count());
    //return binary_search(entries, sample_index,
    return interpolated_search(entries, sample_index, sample_count(),
            [](const auto &entry) -> const auto & { return entry.sample_index; });
}


inline unsigned long
Media::Samples::ToChunk::at(uint32_t sample_index) const {
    assert(sample_index < sample_count());
    //return binary_search(runs, sample_index,
    return interpolated_search(runs, sample_index, sample_count(),
        [](const auto &entry) -> const auto & { return entry.sample_index; });
}



Media::Samples::Iterator::Iterator::Iterator(const Samples &samples, uint32_t index)
    : Index(samples, index), decoding(*this), composition(*this), chunk(*this) {
    assert(samples.is_sealed()); // has never been sealed
    if (index > samples.count()) {
        throw Profiler::Exception(SStream()
                << "Samples index " << index
                << " shouldn't be larger than entry count " << samples.count());
    } else if (index < samples.count()) {
        decoding.setup();
        if (samples.composition != nullptr) {
            composition.setup();
        }
        chunk.setup();
    } else {
        assert(index == samples.count()); // end
    }
}

void Media::Samples::Iterator::operator++() {
    if (index >= samples.count()) {
        throw Profiler::Exception(SStream()
                << "After incrementation of samples index " << index
                << " it would exceed beyond total samples count " << samples.count());
    } else if (++index < samples.count()) {
        ++decoding;
        if (samples.composition != nullptr) {
            ++composition;
        }
        ++chunk;
    } else {
        assert(index == samples.count()); // end
    }
}


inline void Media::Samples::Iterator::Decoding::setup() {
    entry_index = it.samples.decoding->at(it.index);
    entry = &it.samples.decoding->entries[entry_index];
}

inline void Media::Samples::Iterator::Decoding::operator++() {
    if (entry->count == (it.index - entry->first_sample.index)) {
        entry = &it.samples.decoding->entries[++entry_index];
    }
}



inline void Media::Samples::Iterator::Composition::setup() {
    entry_index = it.samples.composition->at(it.index);
    entry = &it.samples.composition->entries[entry_index];
}

inline void Media::Samples::Iterator::Composition::operator++() {
    if (entry->count == (it.index - entry->sample_index)) {
        entry = &it.samples.composition->entries[++entry_index];
    }
}

inline uint64_t Media::Samples::Iterator::Composition::offset() const {
    return entry->offset;
}


inline void Media::Samples::Iterator::Chunk::setup() {
    assert(it.samples.to_chunk->is_sealed()); // has never been sealed
    run.index = it.samples.to_chunk->at(it.index);
    set_run(it.samples.to_chunk->runs[run.index]);
    const auto left = it.index - run.entry->sample_index;
    index = left / run.entry->samples_per_chunk;
    sample.index = left % run.entry->samples_per_chunk;
    sample.offset += it.samples.size->sum_sizes(it.index - sample.index, sample.index);
}

inline void Media::Samples::Iterator::Chunk::set_run(const ToChunk::Run &entry) {
    this->run.entry = &entry;
    sample.entry = &it.samples.description->all<SampleEntry>()[entry.sample_description_index];
    assert(sample.entry->is_sealed()); // has never been sealed
}

inline void Media::Samples::Iterator::Chunk::operator++() {
    if  (++sample.index == run.entry->samples_per_chunk) {
        sample.index = 0;
        sample.offset = 0;
        if (++index == run.entry->chunks) {
            index = 0;
            set_run(it.samples.to_chunk->runs[++run.index]);
        }
    } else {
        sample.offset += it.samples.size->sample_size(it.index - 1);
    }
}

void Media::Sample::operator >> (OPrintStream &stream) const {
    if (stream.ellipses(it.index, it.samples.count())) { return; }
    stream.iout() << "#" << it.index << ":" << std::endl;

    OPrintStream::Indent indent(stream, std::string(4, ' ')); {
        stream.iout()
            << "size: " << size() << " bytes" << std::endl;

        stream.iout() << "decoding: "
            << "[#" << it.decoding.entry_index << ": "
            << "time: " << decoding_time() << "]" << std::endl;

        stream.iout() << "composition: "
            << "[";
        if (it.samples.composition != nullptr) {
            stream.out << "#" << it.composition.entry_index << ": ";
        }
        stream.out
            << "offset: " << composition_offset() << ", "
            << "time: " << composition_time();
        stream.out << "]" << std::endl;

        stream.iout() << "chunk: "
            << "[#" << it.chunk.run.index << ": "
                << "first_index: " << first_chunk_index_in_run() << ", "
                << "index: " << chunk_index() << ", "
                << "offset: " << chunk_offset()  << " bytes"
                << "]" << std::endl;

        stream.iout() << "offset: "
            << "[" << offset() << " bytes, "
            << "#" << sample_index_in_chunk() << " samples in "
            << "#" << chunk_index_in_run() << " chunck of "
            << samples_per_chunk() << " " << pluralize("samples", samples_per_chunk()) << " in run of "
            << chunk_count_in_run() << " " << pluralize("chunk", chunk_count_in_run())
            << "]" << std::endl;

        stream.iout() << "description: "
            << "[#" << sample_description_index() << ":";
        stream << sample_entry();
        stream.out << "]" << std::endl;

        if (it.samples.dependency != nullptr) {
            stream.iout() << "dependency: [";
            *dependency() >> stream;
            stream.out << "]" << std::endl;
        }
    }
}


}} // namespace mpeg::profiler

