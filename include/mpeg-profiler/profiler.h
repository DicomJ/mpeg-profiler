#ifndef __MPEG_PROFILER__
#define __MPEG_PROFILER__

#include <mpeg-isobase/parser.h>
#include <mpeg-profiler/box/container.h>


namespace mpeg {
namespace profiler {


struct Profiler: mpeg::isobase::Parser::Observer {

    struct Exception: std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    const Box::Map &boxes;
    Profiler(const Box::Map &boxes = Box::all_boxes);

    //bitstream::Parser::Observer
    virtual void event(const bitstream::Parser::Event::Exception &);
    virtual void event(const bitstream::Parser::Event::Header &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::Begin &);
    virtual void event(const bitstream::Parser::Event::Payload::Data &);
    virtual void event(const bitstream::Parser::Event::Payload::Boundary::End &);

    std::unique_ptr<Container> detach_container();

    bool ignore_exceptions = false;     // and so continue
protected:
    virtual void handle_exception(bool ignore_exception);
    void handle_exception() { handle_exception(ignore_exceptions); }

    std::unique_ptr<Container> container;
    Box *current_box;
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER__

