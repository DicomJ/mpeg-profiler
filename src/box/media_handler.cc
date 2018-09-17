#include <bitstream/opstream.h>
#include <mpeg-isobase/box/handler.h>
#include <mpeg-profiler/box/media_handler.h>


namespace mpeg {
namespace profiler {


void Media::Handler::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::HandlerBox>(event);

    handler_type = box.handler_type;
    name = box.name;
}

void Media::Handler::operator >> (isobase::HandlerBox &box) const {
    FullBox::operator >> (box);

    box.handler_type = handler_type;
    box.name = name;
}

void Media::Handler::operator >> (OPrintStream &stream) const {
    stream.out << name;
}


}} // namespace mpeg::profiler

