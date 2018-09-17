#include <bitstream/opstream.h>
#include <mpeg-profiler/box/movie.h>


namespace mpeg {
namespace profiler {


void Movie::operator >> (OPrintStream &stream) const {
    stream.iout(stream.branching) << "Movie [" ;
    stream << (*this)['mvhd'];
    stream.out << "]" << std::endl;
    for (const auto &track: all('trak')) {
        OPrintStream::Indent indent(stream);
        stream << track;
    }
}


}} // namespace mpeg::profiler

