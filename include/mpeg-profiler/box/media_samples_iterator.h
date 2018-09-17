#ifndef __MPEG_PROFILER_BOX_MEDIA_SAMPLES_ITERATOR__
#define __MPEG_PROFILER_BOX_MEDIA_SAMPLES_ITERATOR__

#include <mpeg-profiler/box/media_samples.h>

#include <mpeg-profiler/box/sample_entry.h>
#include <mpeg-profiler/box/media_samples_size.h>
#include <mpeg-profiler/box/media_samples_decoding_time.h>
#include <mpeg-profiler/box/media_samples_composition_offset.h>
#include <mpeg-profiler/box/media_samples_chunk_offset.h>
#include <mpeg-profiler/box/media_samples_tochunk.h>
#include <mpeg-profiler/box/media_samples_sync.h>
#include <mpeg-profiler/box/media_samples_dependency_type.h>


namespace mpeg {
namespace profiler {


struct Media::Samples::Index {
    Index(const Samples &samples, uint32_t index): samples(samples), index(index) { }

    operator uint32_t () const { return index; }
    long operator - (const Index &index) const { return this->index - index.index; }

    bool operator != (const Index &index) const { return !(*this == index); }
    bool operator == (const Index &index) const {
        assert(&samples == &index.samples);
        return this->index == index.index;
    }

    const Samples &samples;

protected:
    uint32_t index;
};

struct Media::Samples::Iterator : Media::Samples::Index {
    friend struct Media::Sample;

    Iterator(const Samples &samples, uint32_t index);

    void operator++();
    Media::Sample operator * () const;

private:
    struct Decoding {
        const Iterator &it;
        Decoding(const Iterator &it): it(it) {}
        void setup();
        void operator++();

        uint64_t time() const {
            return entry->first_sample.time + entry->delta * (it.index - entry->first_sample.index);
        }

        const DecodingTime::Entry *entry;
        uint32_t entry_index;

        bool operator == (const Decoding &decoding) const {
            return entry_index == decoding.entry_index && entry == decoding.entry;
        }

    } decoding;

    struct Composition {
        const Iterator &it;
        Composition(const Iterator &it): it(it) {}
        void setup();
        void operator++();

        uint64_t offset() const;

        const CompositionOffset::Entry *entry;
        uint32_t entry_index;

        bool operator == (const Composition &composition) const {
            return
                entry_index == composition.entry_index &&
                entry == composition.entry;
        }

    } composition;

    struct Chunk {
        const Iterator &it;
        Chunk(const Iterator &it): it(it) {}
        void setup();
        void set_run(const ToChunk::Run &run);
        void operator++();

        uint32_t absolute_index() const { return run.entry->first_chunk_index + index; }

        uint32_t index = 0;     // chunk index within run

        struct Run {
            const ToChunk::Run *entry;
            uint32_t index;

            bool operator == (const Run &run) const {
                return
                    entry == run.entry &&
                    index == run.index;
            }
        } run;

        struct Sample {
            const SampleEntry *entry = nullptr;   // corresponds to sample_description_index
            uint32_t index = 0;
            uint64_t offset = 0; // sum of sample sizes sum(sample.size[0..index])

            bool operator == (const Sample &sample) const {
                return
                    entry == sample.entry &&
                    index == sample.index &&
                    offset == sample.offset;
            }
        } sample;               // within chunk

        bool operator == (const Chunk &chunk) const {
            return
                index == chunk.index &&
                run == chunk.run &&
                sample == chunk.sample;
        }

    } chunk;

    bool equal(const Iterator &it) const { // More comprehensive than operator ==
        return
            Index::operator == (it) &&
            decoding == it.decoding &&
            (samples.composition != nullptr ? composition == it.composition: true) &&
            chunk == it.chunk;
    }
};


inline Media::Samples::Iterator Media::Samples::at(uint32_t index) const {
    return Iterator(*this, index);
}

inline Media::Samples::Iterator Media::Samples::begin() const {
    return at(0);
}

inline Media::Samples::Index Media::Samples::end() const {
    return Index(*this, sample_count);
}


struct Media::Sample {
    Sample(const Media::Samples::Iterator &it): it(it) {}
    void operator >> (OPrintStream &) const;

    uint32_t size() const { return it.samples.size->sample_size(it.index); }

    Time decoding_time() const { return Time{it.decoding.time(), it.samples.timescale()}; }
    Time composition_offset() const {
        return Time{it.samples.composition != nullptr ? it.composition.offset() : 0, it.samples.timescale()};
    }
    Time composition_time() const { return decoding_time() + composition_offset(); }

    uint32_t chunk_count_in_run() const { return it.chunk.run.entry->chunks; }
    uint32_t chunk_index_in_run() const { return it.chunk.index; }
    uint32_t first_chunk_index_in_run() const { return it.chunk.run.entry->first_chunk_index; }
    uint32_t chunk_index() const { return it.chunk.absolute_index(); }
    uint32_t sample_index_in_chunk() const { return it.chunk.sample.index; }
    uint32_t samples_per_chunk() const { return it.chunk.run.entry->samples_per_chunk; }

    uint64_t chunk_offset() const { return it.samples.chunks->chunk_offset(chunk_index()); }
    uint64_t samples_offset_in_chunk() const { return it.chunk.sample.offset; }
    uint64_t offset() const { return chunk_offset() + samples_offset_in_chunk(); }

    uint32_t sample_description_index() const { return it.chunk.run.entry->sample_description_index; }
    const SampleEntry &sample_entry() const { return *it.chunk.sample.entry; }

    const Media::Samples::DependencyType::Entry *dependency() const {
        return it.samples.dependency != nullptr? &it.samples.dependency->entries[it.index]: nullptr;
    }

    bool operator == (const Sample &sample) const {
        return
            it.equal(sample.it) &&
            size() == sample.size() &&
            composition_time() == sample.composition_time() &&
            offset() == sample.offset() &&
            dependency() == sample.dependency();
    }

private:
    const Media::Samples::Iterator &it;
};


inline Media::Sample
Media::Samples::Iterator::operator * () const {
    return Media::Sample(*this);
}

}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_SAMPLES_ITERATOR__


