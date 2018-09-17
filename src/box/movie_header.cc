#include <bitstream/opstream.h>
#include <mpeg-isobase/box/movie_header.h>
#include <mpeg-profiler/box/movie_header.h>


namespace mpeg {
namespace profiler {


using bitstream::output::print::gtmtime1904_to_string;


void Movie::Header::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    auto &box = cast_header<isobase::MovieHeaderBox>(event);

    creation_time = box.creation_time();
    modification_time = box.modification_time();
    duration = Time{box.duration(), box.timescale()};
    rate = box.rate;
    volume = box.volume;
    next_track_ID = box.next_track_ID;
}

void Movie::Header::operator >> (isobase::MovieHeaderBox &box) const {

    FullBox::operator >> (box);

    box.creation_time(creation_time);
    box.modification_time(modification_time);
    box.timescale(duration.scale);
    box.duration(duration.time);
    box.rate = rate;
    box.volume = volume;
    box.next_track_ID = next_track_ID;
}

void Movie::Header::operator >> (OPrintStream &stream) const {
    stream.out
        << "duration: " << duration << ", "
        << "created: "  << gtmtime1904_to_string(creation_time) << ", "
        << "modified: " << gtmtime1904_to_string(modification_time);
}


}} // namespace mpeg::profiler

