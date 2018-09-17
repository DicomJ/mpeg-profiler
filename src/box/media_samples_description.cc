#include <bitstream/opstream.h>
#include <mpeg-isobase/box/sample_description.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box_constructor.h>
#include <mpeg-profiler/box/media_samples_description.h>
#include <mpeg-profiler/box/media_handler.h>
#include <mpeg-profiler/box/visual_sample_entry.h>
#include <mpeg-profiler/box/audio_sample_entry.h>
#include <mpeg-profiler/box/hint_sample_entry.h>


namespace mpeg {
namespace profiler {


void Media::Samples::Description::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::SampleDescriptionBox>(event);

    _entry_count = box.entry_count;
}

std::unique_ptr<Box> Media::Samples::Description::create_child_box(const isobase::Box &box, Box::Constructor constructor) {
    if(constructor == Box::default_constructor) {
        // TODO make it an external registry
        switch (parent.parent.parent.as<Media::Handler>('hdlr').handler_type) {
            case 'vide': constructor = Box::Constructor::for_box<VisualSampleEntry>(); break;
            case 'soun': constructor = Box::Constructor::for_box<AudioSampleEntry>(); break;
            case 'hint': constructor = Box::Constructor::for_box<HintSampleEntry>(); break;
            default:;
        }
    }
    return Box::create_child_box(box, constructor);
}

void Media::Samples::Description::seal() {
    if (children.empty()) {
        throw Profiler::Exception("No entries in SampleDescriptionBox's been discovered");
    }
}

void Media::Samples::Description::operator >> (isobase::SampleDescriptionBox &box) const {
    FullBox::operator >> (box);

    box.entry_count = children.size();  // Disregards _entry_count
}

void Media::Samples::Description::operator >> (OPrintStream &stream) const {
    stream.out << "Descriptions: [";
    stream.out << children.size() << " " << (children.size() == 1 ? "entry" : "entries");
    if (children.size() == 1) {
        stream.out << ", ";
        stream << *children[0];
    } else {
        OPrintStream::Indent indent(stream, std::string(4, ' '));
        stream.out << std::endl;
        for (const auto &child: children) {
            stream.iout();
            stream << *children[0];
            stream.out << std::endl;
        }
    }
    stream.out << "]";
}


}} // namespace mpeg::profiler

