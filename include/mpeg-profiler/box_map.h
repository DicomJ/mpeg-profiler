#ifndef __MPEG_PROFILER_BOX_MAP__
#define __MPEG_PROFILER_BOX_MAP__

#include <mpeg-profiler/box_constructor.h>


namespace mpeg {
namespace profiler {


struct Box::Map {

    template <typename Type>
    struct Map_: std::unordered_map<Type, Box::Constructor> {
        const Box::Constructor &get(const Type &type, const Box::Constructor &default_constructor) const {
            auto it = this->find(type);
            return it != this->end() ? it->second: default_constructor;
        }
    };

    Map_<isobase::Box::Type> types;
    Map_<std::string> extended_types;

    template<typename IsoBaseBoxType, typename BoxType>
    void add() { add<BoxType>(IsoBaseBoxType::box_type); }
    template<typename BoxType>
    void add(isobase::Box::Type type) { types[type] = Box::Constructor::for_box<BoxType>(); }
    template<typename BoxType>
    void add(const std::string &extended_type) { extended_types[extended_type] = Box::Constructor::for_box<BoxType>(); }

    const Box::Constructor &operator[](isobase::Box::Type type) const { return get(type); }
    const Box::Constructor &operator[](const std::string &extended_type) const { return get(extended_type); }
    const Box::Constructor &operator[](const isobase::Box &box) const { return get(box); }

    const Box::Constructor &get(isobase::Box::Type type, const Box::Constructor &default_constructor = Box::default_constructor) const {
        return types.get(type, default_constructor);
    }
    const Box::Constructor &get(const std::string &extended_type, const Box::Constructor &default_constructor = Box::default_constructor) const {
        return extended_types.get(extended_type, default_constructor);
    }
    const Box::Constructor &get(const isobase::Box &box, const Box::Constructor &default_constructor = Box::default_constructor) const {
        return box.is_usertype() ?
            get(box.usertype(), default_constructor):
            get(box.type(), default_constructor);
    }
};

}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX_MAP__

