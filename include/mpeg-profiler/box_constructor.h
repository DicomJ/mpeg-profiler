#ifndef __MPEG_PROFILER_BOX_CONSTRUCTOR__
#define __MPEG_PROFILER_BOX_CONSTRUCTOR__

#include <mpeg-profiler/box.h>


namespace mpeg {
namespace profiler {


struct Box::Constructor {

    using method_t = std::unique_ptr<Box> (*)(Container &container, Box &parent);
    method_t method;

    template <typename BoxType>
    static Constructor for_box() {
        struct _ { static std::unique_ptr<Box> method(Container &container, Box &parent) {
            return std::make_unique<BoxType>(container, parent);
        }};
        return Constructor{_::method};
    }

    std::unique_ptr<Box> operator ()(Container &container, Box &parent) const {
        return this->method(container, parent);
    }

    bool operator == (const Constructor &constructor) const {
        return method == constructor.method;
    }
};


}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_CONSTRUCTOR__

