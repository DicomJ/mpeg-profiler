#ifndef __MPEG_PROFILER_BOX__
#define __MPEG_PROFILER_BOX__

#include <memory>   // unique_ptr, etc
#include <unordered_map>
#include <bitstream/fixed_point.h>
#include <bitstream/scalable_time.h>
#include <bitstream/parser.h>
#include <mpeg-isobase/box_desc.h>


namespace bitstream {
namespace output {
namespace print {

struct Stream;

}}} // namespace bitstream::output::print


namespace mpeg {
namespace profiler {


using bitstream::FixedPoint;
using bitstream::scalable::Time;
using OPrintStream = bitstream::output::print::Stream;
struct Profiler;
struct Container;


}} // namespace mpeg::profiler


namespace mpeg {
namespace profiler {


struct Box: protected bitstream::Parser::Observer {

    bitstream::Parser::Observer &observer() { return *this; }

    struct Constructor;
    struct Map;

    static const Constructor &default_constructor;
    static const Map &all_boxes;

    Container &container;
    Box &parent;

    isobase::Box::Type type = -1;
    std::string usertype;
    uint64_t size;
    uint64_t offset = 0;

    virtual ~Box() {}
    Box(Container &container, Box &parent) : container(container), parent(parent) {}
    Box(Container &container, Box &parent, isobase::Box::Type type) : container(container), parent(parent), type(type) {}
    Box(Container &container, Box &parent, const std::string &usertype) : container(container), parent(parent), type('uuid'), usertype(usertype) {}
    Box &operator = (const Box &) = delete;

    // bitstream::Parser::Observer
    virtual void event(const bitstream::Parser::Event::Header &event);

    void operator >> (isobase::Box &) const {}
    virtual void operator >> (OPrintStream &) const;
    friend auto &operator << (OPrintStream &ostream, const Box &box) {
        return box >> ostream, ostream;
    }

    virtual void output_caption(OPrintStream &, bool branching=true) const;
    virtual void output_children(OPrintStream &) const;

    template <typename BoxType, typename... Args>
    Box &add(Args&&... args) {
        return add(std::make_unique<BoxType>(*this, std::forward<Args>(args)...));
    }

    Box &add(std::unique_ptr<Box> &&pbox) {
        Box &box = *children.emplace_back(std::move(pbox)).get();
        if (type == 'uuid') {
            children.by_usertype.emplace(box.usertype, &box);
        } else {
            children.by_type.emplace(box.type, &box);
        }
        return box;
    }

    template <typename BoxType = Box>
    BoxType *get(isobase::Box::Type type) const { return children.by_type.get<BoxType>(type); }
    template <typename BoxType = Box>
    BoxType *get(const std::string &usertype) const { return children.by_usertype.get<BoxType>(usertype); }

    Box &operator[](isobase::Box::Type type) const { return children.by_type[type]; }
    Box &operator[](const std::string &usertype) const { return children.by_usertype[usertype]; }

    template <typename BoxType>
    BoxType &as(isobase::Box::Type type) const { return children.by_type.as<BoxType>(type); }
    template <typename BoxType>
    BoxType &as(const std::string &usertype) const { return children.by_usertype.as<BoxType>(type); }

    template <typename BoxType = Box>
    auto all(isobase::Box::Type type) const { return children.by_type.all<BoxType>(type); }
    template <typename BoxType = Box>
    auto all(const std::string &usertype) const { return children.by_usertype.all<BoxType>(usertype); }
    template <typename BoxType = Box>
    auto all() const { return children.all<BoxType>(); }

    template <typename BoxType = Box, typename Predicate>
    auto all(isobase::Box::Type type, const Predicate &predicate) const { return children.by_type.all<BoxType>(type, predicate); }
    template <typename BoxType = Box, typename Predicate>
    auto all(const std::string &usertype, const Predicate &predicate) const { return children.by_usertype.all<BoxType>(usertype, predicate); }
    template <typename BoxType = Box, typename Predicate>
    auto all_of(const Predicate &predicate) const { return children.all<BoxType>(predicate); }

    template <typename BoxType = Box, typename Key>
    BoxType *get_any(const Key &key) const {
        return get<BoxType>(key);
    }

    template <typename BoxType = Box, typename Key, typename ...Args>
    BoxType *get_any(const Key &key, const Args &...args) const {
        BoxType * const box = get<BoxType>(key);
        return box != nullptr ? box : get_any<BoxType>(args...);
    }

    template <typename BoxType = Box, typename ...Args>
    BoxType &any(const Args &...args) const {
        BoxType * const box = get_any<BoxType>(args...);
        if (box == nullptr) {
            bitstream::SStream sstream;
            sstream << "None of the following boxes [";
            mpeg::isobase::Box::UnifiedType::types_to_stream(sstream, args...);
            throw std::out_of_range(sstream << "] found");
        }
        return *box;
    }
    bool has(isobase::Box::Type type) const { return children.by_type.has(type); }
    bool has(const std::string &usertype) const { return children.by_usertype.has(usertype); }

    unsigned long count(isobase::Box::Type type) const { return children.by_type.count(type); }
    unsigned long count(const std::string &usertype) const { return children.by_usertype.count(usertype); }

protected:
    friend Profiler;

    virtual void _seal();
    virtual bool is_sealed() const { return true; }
    virtual void seal() {}

    virtual std::unique_ptr<Box> create_child_box(const isobase::Box &box, Constructor constructor);

    template <typename BaseIterator, typename BoxType>
    struct Range;

    struct Predicated {
        template <typename BaseIterator, typename BoxType, typename Predicate>
        struct Range;
    };

    struct Children: std::vector<std::unique_ptr<Box>> {

        using Base = std::vector<std::unique_ptr<Box>>;

        struct Iterator : Base::const_iterator {
            Iterator(const typename Base::const_iterator &it): Base::const_iterator(it) {}
            Box &operator *() const {
                return *Base::const_iterator::operator * ();
            }
        };

        template <typename BoxType>
        using Range = Box::Range<Iterator, BoxType>;

        struct Predicated {
            template <typename BoxType, typename Predicate>
            using Range = Box::Predicated::Range<Iterator, BoxType, Predicate>;
        };

        template <typename BoxType, typename Predicate>
        auto all(const Predicate &predicate) const {
            return typename Predicated::template Range<BoxType, Predicate>(begin(), end(), predicate);
        }

        template <typename BoxType>
        auto all() const {
            return Range<BoxType>(begin(), end());
        }

        template <typename IndexType>
        struct Index : std::unordered_multimap<IndexType, Box *> {

            using Base = std::unordered_multimap<IndexType, Box *>;

            struct Iterator : Base::const_iterator {
                Iterator(const typename Base::const_iterator &it): Base::const_iterator(it) {}
                Box &operator *() const {
                    return *(*this)->second;
                }
            };

            template <typename BoxType>
            using Range = Box::Range<Iterator, BoxType>;

            struct Predicated {
                template <typename BoxType, typename Predicate>
                using Range = Box::Predicated::Range<Iterator, BoxType, Predicate>;
            };

            bool has(const IndexType &key) const {
                return this->find(key) != this->end();
            }

            Box &operator[](const IndexType &key) const { return as(key); }

            template <typename BoxType = Box>
            BoxType &as(const IndexType &key) const {
                BoxType * const box = get<BoxType>(key);
                if (box == nullptr) {
                    throw std::out_of_range(bitstream::SStream() <<
                        "'" << mpeg::isobase::Box::UnifiedType(key).to_string() << "' Box not found");
                }
                return *box;
            }

            template <typename BoxType>
            BoxType *get(const IndexType &key) const {
                auto range = this->equal_range(key);
                if (range.first != range.second) {
                    Box* box = (range.first++)->second;
                    if (range.first != range.second) {
                    throw std::out_of_range(bitstream::SStream() <<
                        "More than one'" << mpeg::isobase::Box::UnifiedType(key).to_string() << "' Box found");
                    }
                    return &bitstream::dcast<BoxType>(*box);
                } else {
                    return nullptr;
                }
            }

            template <typename BoxType, typename Predicate>
            auto all(const IndexType &key, const Predicate &predicate) const {
                const auto &range = this->equal_range(key);
                return typename Predicated::template Range<BoxType, Predicate>(range.first, range.second, predicate);
            }

            template <typename BoxType>
            auto all(const IndexType &key) const {
                const auto &range = this->equal_range(key);
                return Range<BoxType>(range.first, range.second);
            }
        };

        Index<isobase::Box::Type> by_type;
        Index<std::string> by_usertype;

    } children;
};


template <typename BaseIterator, typename BoxType>
struct Box::Range {
    std::pair<BaseIterator, BaseIterator> range;

    Range(const BaseIterator &begin, const BaseIterator &end)
        : range(begin, end) {}

        struct Iterator {
            BaseIterator it;

            Iterator(const BaseIterator &it) : it(it) {}

            auto &operator ++() { ++it; return *this; }

            Iterator operator + (unsigned long index) { return BaseIterator(it + index); }

            bool operator == (const Iterator &iterator) const { return it == iterator.it; }
            bool operator != (const Iterator &iterator) const { return it != iterator.it; }

            BoxType &operator * () const { return bitstream::dcast<BoxType>(*it); }
        };
    auto size() const { return range.second - range.first; }
    BoxType &operator [] (unsigned long index) { return *(begin() + index); }

    Iterator begin() const { return range.first; }
    Iterator end() const { return range.second; }
};


template <typename BaseIterator, typename BoxType, typename Predicate>
struct Box::Predicated::Range {
    using BaseRange = std::pair<BaseIterator, BaseIterator>;
    BaseRange base_range;
    Predicate predicate;

    Range(const BaseIterator &begin, const BaseIterator &end, const Predicate &predicate)
        : base_range(begin, end), predicate(predicate) {}

    struct Iterator {
        BaseIterator it;
        const Range &range;

        Iterator(const BaseIterator &it, const Range &range)
            : it(it), range(range) {}

        auto &operator ++() {
            for (++it; it != range.base_range.second; ++it) {
                if (range.predicate(**this)) {
                    break;
                }
            }
            return *this;
        }

        bool operator == (const Iterator &iterator) const { return it == iterator.it; }
        bool operator != (const Iterator &iterator) const { return it != iterator.it; }

        BoxType &operator * () const { return bitstream::dcast<BoxType>(*it); }
    };

    Iterator begin() const { return Iterator(base_range.first, *this); }
    Iterator end() const { return Iterator(base_range.second, *this); }
};

}} // namespace mpeg::profiler


#endif // __MPEG_PROFILER_BOX__

