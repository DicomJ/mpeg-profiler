#include <bitstream/opstream.h>
#include <mpeg-isobase/box/file_type.h>
#include <mpeg-profiler/box/container_type.h>


namespace mpeg {
namespace profiler {


using bitstream::output::print::fourcc;


void Container::Type::event(const bitstream::Parser::Event::Header &event) {
    Box::event(event);
    auto &box = cast_header<isobase::FileTypeBox>(event);

    major_brand = box.major_brand;
    minor_version = box.minor_version;
    compatible_brands = box.compatible_brands;

}

void Container::Type::operator >> (isobase::FileTypeBox &box) const {

    Box::operator >> (box);

    box.major_brand = major_brand;
    box.minor_version = minor_version;
    box.compatible_brands = compatible_brands;
}

void Container::Type::operator >> (OPrintStream &stream) const {
    stream.out
        << "major_brand: "  << fourcc(major_brand) << ", "
        << "minor_version: "  << minor_version << ", "
        << "compatible_brands: [";
    bool first = true;
    for (auto brand: compatible_brands) {
        if (!first) { stream.out << ", "; } first = false;
        stream.out << fourcc(brand);
    }
    stream.out << "]";
}


}} // namespace mpeg::profiler

