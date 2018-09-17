#include <bitstream/opstream.h>
#include <mpeg-profiler/box.h>
#include <mpeg-profiler/box_constructor.h>


namespace mpeg {
namespace profiler {


using bitstream::output::print::fourcc;


const Box::Constructor &Box::default_constructor = Box::Constructor::for_box<Box>();


void Box::event(const bitstream::Parser::Event::Header &event) {
    const auto &box = cast_header<isobase::Box>(event);
    type = box.type();
    usertype = box.usertype();
    size = box.size();
    offset = event.parser.hstream.stream.offset();
}

void Box::output_caption(OPrintStream &stream, bool branching) const {
    if (branching) {
        stream.iout(stream.branching);
    }
    stream.out << "Box(";
    if (type == 'uuid') {
        stream.out << usertype;
    } else {
        stream.out << fourcc(type);
    }
    stream.out << ")   [" << size << " bytes] @" << offset << std::endl;
}

void Box::output_children(OPrintStream &stream) const {
    OPrintStream::Indent indent(stream); {
        for (const auto &child: children) {
            stream << *child;
        }
    }
}

void Box::operator >> (OPrintStream &stream) const {
    output_caption(stream);
    output_children(stream);
}

void Box::_seal() {
    seal();
    for (const auto &child: children) {
        child->_seal();
    }
}

std::unique_ptr<Box> Box::create_child_box(const isobase::Box &box, Box::Constructor constructor) {
    return constructor(container, *this);
}


}} // namespace mpeg::profiler
