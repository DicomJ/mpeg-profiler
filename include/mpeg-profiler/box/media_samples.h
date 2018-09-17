#ifndef __MPEG_PROFILER_BOX_MEDIA_SAMPLES__
#define __MPEG_PROFILER_BOX_MEDIA_SAMPLES__

#include <mpeg-profiler/box/media.h>

#include <algorithm> // TODO: clang++ rejects forward declaration of std::lower_bound
namespace std {
//template <class ForwardIterator, class T, class Compare>
//ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T &val, Compare comp);
} // namespace std


namespace mpeg {
namespace profiler {


struct Media::Samples: Box {

    struct Description;
    struct Size;
    struct RegularSize;
    struct CompactSize;
    struct DecodingTime;
    struct CompositionOffset;
    struct ChunkOffset;
    struct ChunkRegularOffset;
    struct ChunkLargeOffset;
    struct ToChunk;
    struct Sync;
    struct DependencyType;

    struct Index;
    struct Iterator;

    Samples(Container &container, Box &parent): Box(container, parent) {}

    virtual void operator >> (OPrintStream &) const;

    Iterator begin() const;
    Index end() const;
    Iterator at(uint32_t index) const;

    Index seek(Time time, bool no_sync=false) const;
    Index seek(double seconds, bool no_sync=false) const;
    Index seek(uint64_t time, bool no_sync=false) const;    // in mdhd timescale

    uint32_t count() const { return sample_count; }
    uint32_t timescale() const { return _timescale; }
    Sync *sync() const { return _sync; }

protected:
    friend struct Media::Sample;
    virtual void seal();
    virtual bool is_sealed() const { return mdhd != nullptr; }

    Media::Header *mdhd = nullptr;
    uint32_t sample_count = 0;
    uint32_t _timescale = 0;

    Description *description = nullptr;
    DecodingTime *decoding = nullptr;
    CompositionOffset *composition = nullptr;           // optional
    Size *size = nullptr;
    ChunkOffset *chunks = nullptr;
    ToChunk *to_chunk = nullptr;
    Sync *_sync = nullptr;                              // optional
    DependencyType *dependency = nullptr;               // optional
};


}} // namespace mpeg::profiler


namespace mpeg {
namespace profiler {


template <typename Entries, typename T, typename Map>
auto binary_search(const Entries &entries, const T &value, const Map &map) {
    const auto index = std::lower_bound(entries.begin(), entries.end(), value,
            [&map](const auto &entry, const T &value) -> bool { return map(entry) <= value; }) - entries.begin();
    return index != 0 ? index - 1 : 0;
}

template <typename Entries, typename T>
auto binary_search(const Entries &entries, const T &value) {
    return binary_search(entries, value, [](const T &entry) -> const T & { return entry; });
}


template <typename Entries, typename T, typename Map>
unsigned long interpolated_search(const Entries &entries, const T &value, T last_value, const Map &map) {
    unsigned long first = 0, count = entries.size();
    while (count > 0) {
        T first_value = map(entries[first]);
        if (first_value > value) {
            break;
        }
        auto index = first;
        auto range = last_value - first_value;
        auto step = range ? (unsigned long)(T(count) * (value - first_value) / range) : 0;
        index += step;
        const T &value_at_step = map(entries[index]);
        if (value_at_step <= value) {
            first = index + 1;
            count -= (step + 1);
        } else {
            count = step;
            last_value = value_at_step;
        }
    }
    // Here we found the "first" which is greater than value, so the previous one is either less or equal
    first = first != 0 ? first - 1 : 0;
    //assert(first == binary_search(entries, value, map));
    return first;
}

template <typename Entries, typename T>
unsigned long interpolated_search(const Entries &entries, const T &value, const T &last_value) {
    return interpolated_search(entries, value, last_value, [](const T &entry) -> const T & { return entry; });
}

}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MEDIA_SAMPLES__

