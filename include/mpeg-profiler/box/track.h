#ifndef __MPEG_PROFILER_BOX_TRACK__
#define __MPEG_PROFILER_BOX_TRACK__

#include <mpeg-profiler/box.h>
#include <mpeg-profiler/box/media.h>


namespace mpeg {
namespace profiler {


struct EditList;


struct Track: Box {  // trak

    struct Header;

    Track(Container &container, Box &parent): Box(container, parent) {}
    virtual void operator >> (OPrintStream &) const;

    bool is(isobase::Box::Type handler_type) const;
    bool is_video() const { return is('vide'); }
    bool is_audio() const { return is('soun'); }
    bool is_hint() const { return is('hint'); }

    Media::Samples &samples() const;
    EditList *edit_list() const;
};

}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_TRACK__

