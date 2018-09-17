#include <bitstream/opstream.h>
#include <mpeg-isobase/box/data_entry_url.h>
#include <mpeg-profiler/box/data_entry_url.h>


namespace mpeg {
namespace profiler {


void DataEntryUrl::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::DataEntryUrlBox>(event);

    if (box.flags & isobase::DataEntryUrlBox::SameFileFlag) {
        location = box.location;
    }
}

void DataEntryUrl::operator >> (isobase::DataEntryUrlBox &box) const {
}

void DataEntryUrl::operator >> (OPrintStream &stream) const {
}



}} // namespace mpeg::profiler

