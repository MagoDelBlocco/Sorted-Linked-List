#pragma once

template <class _elem, typename comp_funct>
class linked_list {
public:  // smecheria e ca nu pun iteratoru la loc dupa accesari
    linked_list() {
        iterator = begin();
        length &= 0;
        node::link(header, sentinel);
    }

    linked_list(const linked_list<_elem, comp_funct>& target) {
        *this = target;
    }

    linked_list& operator = (const linked_list<_elem, comp_funct>& target) {
        purge();

        length = target.length;
        iterator = target.begin().next;
        BDIter auxiliary = begin();
        node* new_node;

        while (iterator.position != target.end()) {
            new_node = new node(*iterator.position);
            node::link(*auxiliary.position, *new_node);
            ++auxiliary;
            ++iterator;
        }

        node::link(*auxiliary.position, *end());
        iterator = begin();
    }

    _elem& operator[] (const unsigned pos) {
        auto direction = [this, pos]() {
            return this->iterator.offset > pos ? -1 : 1;
        };

        while (pos != iterator.offset) {
            if (direction() == 1)
                ++iterator;
            else
                --iterator;
        }

        return iterator.position->info;
    }

    void add(_elem target) {
        iterator = get_position(target);

        node* new_node = new node(target);

        node::enchain(*iterator.position, *new_node,
            *iterator.position->next);

        ++length;
        iterator = begin();
    }

    void remove(_elem target) {
        iterator = get_position(target);
        ++iterator;

        node* to_be_deleted = iterator.position;

        node::link(*iterator.position->prev, *iterator.position->next);

        delete to_be_deleted;

        --length;
        iterator = begin();
    }

    size_t size() {
        return length;
    }

    ~linked_list() {
        purge();
    }

private:
    struct node {
        _elem info;
        node* next, * prev;

        node() {}

        explicit node(const _elem& target) {
            info = target;
        }

        explicit node(const node& target) {
            *this = target;
        }

        node& operator = (const node& target) {
            info = target.info;
        }

        static inline void link(node& left, node& right) {
            left.next = &right;
            right.prev = &left;
        }

        static void enchain(node& left, node& mid, node& right) {
            link(left, mid);
            link(mid, right);
        }
    } header, sentinel;

    struct BDIter {
        node* position;
        unsigned offset;

        BDIter() {}

        explicit BDIter(node* target) {
            position = target;
            offset = 0;
        }

        BDIter& operator = (const BDIter& target) {
            position = target.position;
            offset = target.offset;

            return *this;
        }

        BDIter& operator = (node* target) {  // used only with begin()
            position = target;
            offset &= 0;

            return *this;
        }

        void operator++ () {
            position = position->next;
            ++offset;
        }

        void operator-- () {
            position = position->prev;
            --offset;
        }

        bool operator == (node* target) {
            return position == target;
        }

        bool operator != (node* target) {
            return (position == target) ^ 1;
        }

        ~BDIter() {}
    } iterator;

    void purge() {
        BDIter next;

        for (iterator = begin()->next; iterator != end();) {
            next = iterator.position->next;
            delete iterator.position;
            iterator = next;
        }

        node::link(header, sentinel);

        length &= 0;
    }

    [[gnu::pure]] inline int clz(size_t target) {
        if (target == 0)
            return 0;
        else
            return (sizeof(target) << 3) - __builtin_clz(target);
    }

    BDIter get_position(_elem target) {  // paced binary search
        unsigned pace = length == 0 ? 0 : (1 << (clz(length) - 1)),
            needed_offset = 0;

        BDIter retval;
        retval = begin();

        for (; pace; pace >>= 1) {
            if (needed_offset + pace > length)
                continue;
            if (comparator((*this)[needed_offset + pace], target))
                needed_offset += pace;
        }

        while (needed_offset != retval.offset) {
            ++retval;
        }

        return retval;
    }

    node* begin() {
        return &header;
    }

    node* end() {
        return &sentinel;
    }

    size_t length;
    comp_funct comparator;
};
