#include <bitstream/opstream.h>
#include <mpeg-profiler/box/track.h>
#include <mpeg-profiler/box/edit_list.h>
#include <mpeg-profiler/box/media_handler.h>
#include <mpeg-profiler/box/media_samples.h>


namespace mpeg {
namespace profiler {


Media::Samples &Track::samples() const {
    return (*this)['mdia']['minf'].as<Media::Samples>('stbl');
}

EditList *Track::edit_list() const {
    auto edit = get('edts');
    return edit != nullptr ? &edit->as<EditList>('elst'): nullptr;
}

bool Track::is(isobase::Box::Type handler_type) const {
    return (*this)['mdia'].as<Media::Handler>('hdlr').handler_type == handler_type;
}

void Track::operator >> (OPrintStream &stream) const {
    stream.iout(stream.branching) <<  "Track(";
    stream << (*this)['mdia']['hdlr'];
    stream.out << ") [";
    stream << (*this)['tkhd'];
    stream.out << "]" << std::endl;

    OPrintStream::Indent indent(stream); {
        auto edit_list = this->edit_list();
        if (edit_list != nullptr) {
            stream << *edit_list;
        }
        stream << (*this)['mdia'];
    }
    //Box::operator >> (stream);
}


}} // namespace mpeg::profiler

