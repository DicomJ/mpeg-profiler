#include <bitstream/opstream.h>
#include <mpeg-isobase/box/media_header.h>
#include <mpeg-profiler/box/media_header.h>


namespace mpeg {
namespace profiler {


using bitstream::output::print::gtmtime1904_to_string;
using bitstream::output::print::ISO_639_2_T_code;
using bitstream::output::print::ISO_639_2_T_name;


void Media::Header::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    auto &box = cast_header<isobase::MediaHeaderBox>(event);

    creation_time = box.creation_time();
    modification_time = box.modification_time();
    duration = Time{box.duration(), box.timescale()};
    language = box.language;
}

void Media::Header::operator >> (isobase::MediaHeaderBox &box) const {
    FullBox::operator >> (box);

    box.creation_time(creation_time);
    box.modification_time(modification_time);
    box.timescale(duration.scale);
    box.duration(duration.time);
    box.language = language;
}

void Media::Header::operator >> (OPrintStream &stream) const {
    auto lang_code = ISO_639_2_T_code(language);
    auto lang_name = ISO_639_2_T_name(lang_code);
    stream.out
        << "duration: " << duration << ", "
        << "language: " << (lang_name.empty() ? lang_code : lang_name) << ", "
        << "created: "  << gtmtime1904_to_string(creation_time) << ", "
        << "modified: " << bitstream::output::print::gtmtime1904_to_string(modification_time);
}


}} // namespace mpeg::profiler

