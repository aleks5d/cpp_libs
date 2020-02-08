#include<cstddef>
#include <stdexcept>
#include<initializer_list>
#include<list>
#include<vector>

#include <iostream>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> > class HashMap {
private:
    std::vector<std::vector<std::pair<std::pair<KeyType, ValueType>, size_t>>> table;
    std::vector<std::pair<size_t, size_t>> elements;
    Hash hasher;
    int sz;
    int  mySize;

    int getHash(KeyType key) const {
        return hasher(key) & (mySize - 1);
    }

    void add(std::pair<KeyType, ValueType> el) {
        int hs = getHash(el.first);
        table[hs].push_back({el, elements.size()});
        elements.push_back({hs, table[hs].size() - 1});
        ++sz;
        if ((sz << 1) > mySize)
            rebuild();
    }

    bool inMe(KeyType& key) const {
        int hs = getHash(key);
        for (auto& i : table[hs]) {
            if (i.first.first == key)
                return 1;
        }
        return 0;
    }

    void del(KeyType key) {
        int hs = getHash(key);
        int ind = 0;
        while (!(table[hs][ind].first.first == key))
            ++ind;
        int ind_el = table[hs][ind].second;
        std::swap(elements.back(), elements[ind_el]);
        table[elements[ind_el].first][elements[ind_el].second].second = ind_el;
        table[hs][ind].second = elements.size() - 1;
        std::swap(table[hs][ind], table[hs].back());
        elements[table[hs][ind].second].second = ind;
        table[hs].pop_back();
        elements.pop_back();
        --sz;
    }

    void rebuild() {
        std::vector<std::pair<KeyType, ValueType>>arr(sz);

        for (auto i : elements) {
            arr[--sz] = (table[i.first][i.second].first);
        }
        for (auto i : elements) {
            if (table[i.first].size())
                table[i.first].clear();
        }
        elements.clear();
        mySize <<= 1;
        table.resize(mySize);
        for (auto &i : arr) {
            add(i);
        }
    }
public:
    //iterators
    class const_iterator;
    class iterator {
    public:
        int me;
        HashMap* to;
        iterator(HashMap* _to = nullptr, int x = 0) {
            to = _to;
            me = x;
        }
        iterator operator++(int) {
            iterator x = *this;
            me++;
            return x;
        }
        iterator operator--(int) {
            iterator x = *this;
            me--;
            return x;
        }
        iterator& operator++() {
            me++;
            return *this;
        }
        iterator& operator--() {
            me--;
            return *this;
        }
        bool operator==(iterator other) const {
            return me == other.me;
        }
        bool operator!=(iterator other) const {
            return me != other.me;
        }
        std::pair<const KeyType, ValueType>& operator*() {
            return reinterpret_cast<std::pair<const KeyType, ValueType>&>(to->get(me));
        }
        std::pair<const KeyType, ValueType>* operator->() {
            return reinterpret_cast<std::pair<const KeyType, ValueType>*>(&to->get(me));
        }
        void operator=(const_iterator other) {
            me = other.me;
            to = other.to;
        }
    };
    class const_iterator {
    public:
        int me;
        const HashMap* to;
        const_iterator(const HashMap* _to = nullptr, int x = 0) {
            to = _to;
            me = x;
        }
        const_iterator operator++(int) {
            const_iterator x = *this;
            me++;
            return x;
        }
        const_iterator operator--(int) {
            const_iterator x = *this;
            me--;
            return x;
        }
        const_iterator& operator++() {
            me++;
            return *this;
        }
        const_iterator& operator--() {
            me--;
            return *this;
        }
        bool operator==(const_iterator other) const {
            return me == other.me && to == other.to;
        }
        bool operator!=(const_iterator other) const {
            return me != other.me || to != other.to;
        }
        const std::pair<const KeyType, ValueType>& operator*() const {
            return reinterpret_cast<const std::pair<const KeyType, ValueType>&>(to->get(me));
        }
        const std::pair<const KeyType, ValueType>* operator->() const {
            return reinterpret_cast<const std::pair<const KeyType, ValueType>*>(&to->get(me));
        }
        void operator=(iterator other) {
            me = other.me;
            to = other.to;
        }
    };
    //part 1
    HashMap(const Hash& hs = Hash()) : hasher(hs) {
        table.resize(2);
        sz = 0;
        mySize = 2;
    }
    //part 2
    template<typename Iter>
    HashMap(Iter beg, Iter en, const Hash& hs = Hash()) {
        table.resize(2);
        sz = 0;
        mySize = 2;
        hasher = hs;
        while (!(beg == en)) {
            insert(*beg);
            beg++;
        }
    }
    //part 3
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list, const Hash& hs = Hash()) {
        table.resize(2);
        sz = 0;
        mySize = 2;
        hasher = hs;
        for (auto i : list)
            insert(i);
    }
    //part 4
    HashMap(const HashMap& to) {
        table = to.table;
        hasher = to.hasher;
        sz = to.sz;
        mySize = to.mySize;
        elements = to.elements;
    }
    //part 5
    int size() const {
        return sz;
    }
    bool empty() const {
        return sz == 0;
    }
    //part 6
    Hash hash_function() const {
        return hasher;
    }
    //part 7
    void insert(std::pair<KeyType, ValueType> element) {
        if (!inMe(element.first)) {
            add(element);
        }
    }
    //part 8
    void erase(KeyType key) {
        if (inMe(key)) {
            del(key);
        }
    }
    //part 9
    iterator begin() {
        return iterator(this, 0);
    }
    iterator end() {
        return iterator(this, elements.size());
    }
    const_iterator begin() const {
        return const_iterator(this, 0);
    }
    const_iterator end() const {
        return const_iterator(this, elements.size());
    }
    //part 10
    iterator find(KeyType key) {
        if (!inMe(key))
            return end();
        int hs = getHash(key);
        int ind = 0;
        while (!(table[hs][ind].first.first == key))
            ++ind;
        return iterator(this, table[hs][ind].second);
    }
    const_iterator find(KeyType key) const{
        if (!inMe(key))
            return end();
        int hs = getHash(key);
        int ind = 0;
        while (!(table[hs][ind].first.first  == key))
            ++ind;
        return const_iterator(this, table[hs][ind].second);

    }
    //part 11
    ValueType& operator[] (KeyType key) {
        if (!inMe(key))
            add({key, ValueType()});
        int hs = getHash(key);
        int ind = 0;
        while (!(table[hs][ind].first.first == key))
            ++ind;
        return table[hs][ind].first.second;
    }
    //part 12
    const ValueType& at(KeyType key) const {
        if (!inMe(key))
            throw std::out_of_range("");
        int hs = getHash(key);
        int ind = 0;
        while (!(table[hs][ind].first.first == key))
            ++ind;
        return table[hs][ind].first.second;
    }
    //part 13
    void clear() {
        while (sz) {
            table[elements.back().first].clear();
            elements.pop_back();
            --sz;
        }
    }
    //part additional
    const std::pair<KeyType, ValueType>& get(int x) const {
        return table[elements[x].first][elements[x].second].first;
    }
    std::pair<KeyType, ValueType>& get(int x) {
        return table[elements[x].first][elements[x].second].first;
    }
};
