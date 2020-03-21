#include<cstddef>
#include <stdexcept>
#include<initializer_list>
#include<list>
#include<vector>

#include <iostream>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> > class HashMap {
private:
    using tableElement = std::pair<KeyType, ValueType>;
    std::vector<std::vector<std::pair<tableElement, size_t>>> table;
    std::vector<std::pair<size_t, size_t>> elements;
    Hash hasher;
    size_t elementsCount;
    size_t tableSize;
    static const size_t defaultSize = 2;
    static const size_t rebuildMultiply = 2;

    size_t getHash(KeyType key) const {
        return hasher(key) & (tableSize - 1);
    }

    void add(tableElement el) {
        size_t hs = getHash(el.first);
        table[hs].push_back({el, elements.size()});
        elements.push_back({hs, table[hs].size() - 1});
        ++elementsCount;
        if ((elementsCount * rebuildMultiply) > tableSize)
            rebuild();
    }

    bool inMe(KeyType& key) const {
        size_t hs = getHash(key);
        for (auto& i : table[hs]) {
            if (i.first.first == key)
                return 1;
        }
        return 0;
    }

    void del(KeyType key) {
        size_t hs = getHash(key);
        size_t ind = 0;
        while (!(table[hs][ind].first.first == key))
            ++ind;
        size_t ind_el = table[hs][ind].second;
        std::swap(elements.back(), elements[ind_el]);
        table[elements[ind_el].first][elements[ind_el].second].second = ind_el;
        table[hs][ind].second = elements.size() - 1;
        std::swap(table[hs][ind], table[hs].back());
        elements[table[hs][ind].second].second = ind;
        table[hs].pop_back();
        elements.pop_back();
        --elementsCount;
    }

    void rebuild() {
        std::vector<tableElement> allElements(elementsCount);

        for (auto i : elements) {
            allElements[--elementsCount] = (table[i.first][i.second].first);
        }
        for (auto i : elements) {
            if (table[i.first].size())
                table[i.first].clear();
        }
        elements.clear();
        tableSize *= rebuildMultiply;
        table.resize(tableSize);
        for (auto &i : allElements) {
            add(i);
        }
    }
public:
    //iterators
    class const_iterator;
    class iterator {
    public:
        size_t me;
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
        size_t me;
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
        table.resize(defaultSize);
        elementsCount = 0;
        tableSize = defaultSize;
    }
    //part 2
    template<typename Iter>
    HashMap(Iter beg, Iter en, const Hash& hs = Hash()) {
        table.resize(defaultSize);
        elementsCount = 0;
        tableSize = defaultSize;
        hasher = hs;
        while (!(beg == en)) {
            insert(*beg);
            beg++;
        }
    }
    //part 3
    HashMap(std::initializer_list<tableElement> list, const Hash& hs = Hash()) {
        table.resize(defaultSize);
        elementsCount = 0;
        tableSize = defaultSize;
        hasher = hs;
        for (auto i : list)
            insert(i);
    }
    //part 4
    HashMap(const HashMap& to) {
        table = to.table;
        hasher = to.hasher;
        elementsCount = to.elementsCount;
        tableSize = to.tableSize;
        elements = to.elements;
    }
    //part 5
    size_t size() const {
        return elementsCount;
    }
    bool empty() const {
        return elementsCount == 0;
    }
    //part 6
    Hash hash_function() const {
        return hasher;
    }
    //part 7
    void insert(tableElement element) {
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
        size_t hs = getHash(key);
        size_t ind = 0;
        while (!(table[hs][ind].first.first == key))
            ++ind;
        return iterator(this, table[hs][ind].second);
    }
    const_iterator find(KeyType key) const{
        if (!inMe(key))
            return end();
        size_t hs = getHash(key);
        size_t ind = 0;
        while (!(table[hs][ind].first.first  == key))
            ++ind;
        return const_iterator(this, table[hs][ind].second);

    }
    //part 11
    ValueType& operator[] (KeyType key) {
        if (!inMe(key))
            add({key, ValueType()});
        size_t hs = getHash(key);
        size_t ind = 0;
        while (!(table[hs][ind].first.first == key))
            ++ind;
        return table[hs][ind].first.second;
    }
    //part 12
    const ValueType& at(KeyType key) const {
        if (!inMe(key))
            throw std::out_of_range("");
        size_t hs = getHash(key);
        size_t ind = 0;
        while (!(table[hs][ind].first.first == key))
            ++ind;
        return table[hs][ind].first.second;
    }
    //part 13
    void clear() {
        while (elements.size()) {
            table[elements.back().first].clear();
            elements.pop_back();
        }
        elementsCount = 0;
    }
    //part additional
    const tableElement& get(int x) const {
        return table[elements[x].first][elements[x].second].first;
    }
    tableElement& get(int x) {
        return table[elements[x].first][elements[x].second].first;
    }
};
