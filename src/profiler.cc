#include <istream>     // TODO: remove this
#include <bitstream/opstream.h>
#include <bitstream/sstream.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box_map.h>
#include <mpeg-profiler/box/failover.h>


namespace mpeg {
namespace profiler {


Profiler::Profiler(const Box::Map &boxes)
    : boxes(boxes)
    , container(std::make_unique<Container>())
    , current_box(container.get())
{
}

std::unique_ptr<Container> Profiler::detach_container() {
    this->container->_seal();
    auto container = std::move(this->container);
    this->container = std::make_unique<Container>();
    current_box = this->container.get();
    return container;
}

void Profiler::event(const bitstream::Parser::Event::Header &event) {
    //bitstream::output::print::Stream::event(event);
    if (const auto *box = dynamic_cast<const isobase::Box *>(&event.header)) {
        try {
            auto new_box = current_box->create_child_box(*box, boxes[*box]);
            new_box->observer().event(event);
            current_box = &current_box->add(std::move(new_box));
        } catch (...) {
            // Failover on ordinary box and if that fails
            try {
                handle_exception();
            } catch (...) {
                // Just caught any exception to make a Failover
                if (!ignore_exceptions) {
                    throw;
                }
            }
            auto new_box = std::make_unique<FailoverBox>(*container, *current_box);
            new_box->observer().event(event);
            current_box = &current_box->add(std::move(new_box));
        }
    }
   // else {
   //     try {
   //         current_box->entry_event(event);
   //     } catch (...) {
   //         handle_exception();
   //     }
   // }
}

void Profiler::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    //bitstream::output::print::Stream::event(event);
    if (const auto *box = dynamic_cast<const isobase::Box *>(&event.header)) {
        try {
            current_box->observer().event(event);
        } catch (...) {
            handle_exception();
        }
    }
}

void Profiler::event(const bitstream::Parser::Event::Payload::Boundary::End &event) {
    //bitstream::output::print::Stream::event(event);
    if (const auto *box = dynamic_cast<const isobase::Box *>(&event.header)) {
        try {
            current_box->observer().event(event);
        } catch (...) {
            handle_exception();
        }
        current_box = &current_box->parent;
    }
}

void Profiler::event(const bitstream::Parser::Event::Payload::Data &event) {
    //bitstream::output::print::Stream::event(event);
    if (const auto *box = dynamic_cast<const isobase::Box *>(&event.header)) {
        try {
            current_box->observer().event(event);
        } catch (...) {
            handle_exception();
        }
    }
}

void Profiler::event(const bitstream::Parser::Event::Exception &) {
    // Caught an exception ignored by parser
    handle_exception(true); // ignore exception
}

void Profiler::handle_exception(bool ignore_exceptions) {
    if (!ignore_exceptions) {
        throw;  // rethrow
    } else {
        // TODO: consider not using stderr: maybe introduce some notification interface?
        try {
            throw;
        } catch (const std::exception &e) {
            std::cout
                << "Profiler: Exception caught while processing handling parsing event: "
                << e.what() << std::endl;
        } catch (...) {
            std::cout
                << "Profiler: unknown exception caught while processing handling parsing event "
                << std::endl;
        }
    }
}


}} // namespace mpeg::profiler

