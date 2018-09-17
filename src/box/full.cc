#include <bitstream/opstream.h>
#include <mpeg-isobase/box/full.h>
#include <mpeg-profiler/box/full.h>


namespace mpeg {
namespace profiler {


void FullBox::event(const bitstream::Parser::Event::Header &event) {
    Box::event(event);
    const auto &box = cast_header<isobase::FullBox>(event);
    flags = box.flags;
    version = box.version;
}

void FullBox::operator >> (isobase::FullBox &box) const {
    Box::operator >>(box);
}

void FullBox::operator >> (OPrintStream &stream) const {
    Box::operator >> (stream);
    OPrintStream::Indent indent(stream, std::string(4, ' ')); {
        stream.iout() << "version: " << std::dec << version << std::endl;
        stream.iout() << "flags: " << std::hex << flags << std::endl;
    }
}


}} // namespace mpeg::profiler

