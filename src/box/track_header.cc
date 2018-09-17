#include <bitstream/opstream.h>
#include <mpeg-isobase/box/track_header.h>
#include <mpeg-profiler/box/track_header.h>
#include <mpeg-profiler/box/movie_header.h>
#include <mpeg-profiler/box/media_handler.h>


namespace mpeg {
namespace profiler {


using bitstream::output::print::gtmtime1904_to_string;


void Track::Header::seal() {
    const auto &mvhd = parent.parent.as<Movie::Header>('mvhd');
    duration.scale = mvhd.duration.scale;
}

void Track::Header::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    auto &box = cast_header<isobase::TrackHeaderBox>(event);

    creation_time = box.creation_time();
    modification_time = box.modification_time();
    track_ID = box.track_ID();
    duration.time = box.duration();
    volume = box.volume;
    width = box.width;
    height = box.height;
}

void Track::Header::operator >> (isobase::TrackHeaderBox &box) const {
    FullBox::operator >> (box);

    box.creation_time(creation_time);
    box.modification_time(modification_time);
    box.track_ID(track_ID);
    box.duration(duration.time);
    box.volume = volume;
    box.width = width;
    box.height = height;
}

void Track::Header::operator >> (OPrintStream &stream) const {
    stream.out
        << "duration: " << duration << ", ";

    if (parent['mdia'].as<Media::Handler>('hdlr').handler_type == 'vide') {
        stream.out
            << "resolution: " << double(width) << "x" << double(height) << ", ";
    }
    stream.out
        << "created: "  << gtmtime1904_to_string(creation_time) << ", "
        << "modified: " << gtmtime1904_to_string(modification_time);
}


}} // namespace mpeg::profiler

