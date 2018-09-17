#ifndef __MPEG_PROFILER_BOX_CONTAINER__
#define __MPEG_PROFILER_BOX_CONTAINER__

#include <mpeg-profiler/box.h>


namespace mpeg {
namespace profiler {


struct Track;


struct Container: Box {
    struct Type;
    struct Data;

    Container(): Box(*this, *this, isobase::Box::top_level_box) {}
    virtual void operator >> (OPrintStream &stream) const;

    struct {
        operator uint64_t () const { return value; }
        auto &operator += (uint64_t value) { this->value += value; return *this; }
        private: uint64_t value = 0;
    } data_size;

    auto tracks() const         { return (*this)['moov'].all<Track>('trak'); }
    auto video_tracks() const   { return (*this)['moov'].all<Track>('trak', [](const auto &track) { return track.is_video(); }); }
    auto audio_tracks() const   { return (*this)['moov'].all<Track>('trak', [](const auto &track) { return track.is_audio(); }); }
    auto hint_tracks() const    { return (*this)['moov'].all<Track>('trak', [](const auto &track) { return track.is_hint();  }); }
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_CONTAINER__

