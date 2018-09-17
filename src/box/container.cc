#include <iomanip>
#include <bitstream/opstream.h>
#include <mpeg-profiler/box/container.h>


namespace mpeg {
namespace profiler {


using bitstream::output::print::pluralize;


void Container::operator >> (OPrintStream &stream) const {
    stream.out << std::setprecision(2) << std::fixed;
    stream.iout() << "Container: [";
    stream << any('ftyp', 'styp');
    stream.out << "]" << std::endl;
    long mdat_count = count('mdat');
    stream.iout() << "Data: [size: " << data_size <<" bytes in " << mdat_count << " 'mdat' " << pluralize("box", mdat_count) << "]" << std::endl;
    stream << (*this)['moov'];
}


}} // namespace mpeg::profiler

