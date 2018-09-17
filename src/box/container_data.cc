#include <bitstream/opstream.h>
#include <mpeg-profiler/box/container_data.h>

namespace mpeg {
namespace profiler {


void Container::Data::event(const bitstream::Parser::Event::Header &event) {
    Box::event(event);
    container.data_size += size;
}

void Container::Data::operator >> (OPrintStream &stream) const {
    stream.iout()
        << "Data: [" << "size: " << size << " bytes]" << std::endl;
}


}} // namespace mpeg::profiler

