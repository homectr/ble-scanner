#pragma once

template <class T>
struct ListEntry
{
    T *entry;
    ListEntry *next;
};

template <class T>
class List
{
    ListEntry<T> *list = nullptr;
    unsigned int _length = 0;

public:
    void add(T *entry);
    ListEntry<T> *getList() { return list; };
    unsigned int length() { return _length; };
};

template <class T>
void List<T>::add(T *entry)
{
    ListEntry<T> *le = new ListEntry<T>();
    le->entry = entry;
    le->next = list;
    list = le;
    _length++;
}