//
// govnocode created by chap4ev on 14.04.2021
//

#ifndef HOMEWORK_2_STATIC_ARRAY_H
#define HOMEWORK_2_STATIC_ARRAY_H

#include <stdexcept>
#include <memory>
#include <vector>

template<typename T, size_t sz = 0>
class static_array {
 public:
  class iterator {
   protected:
    friend class static_array;
    T* _item;
    static_array<T, sz>* _owner;
    iterator(T* item, static_array<T, sz>* owner) : _item(item), _owner(owner) { };

   public:
    iterator(const iterator& other) : _item(other._item) { };

    ~iterator() = default;

    iterator& operator=(const iterator& other) {
      _item = other._item;
      return *this;
    }

    iterator& operator++() {
      _item = reinterpret_cast<T*>(_owner->_data) +
              _owner->_get_next_avaliable_index(_item - reinterpret_cast<T*>(_owner->_data));
      return *this;
    }

    iterator& operator--() {
      auto prev_index = _owner->_get_prev_avaliable_index(_item - _owner->_data);
      if (prev_index == -1) {
        _item = reinterpret_cast<T*>(_owner->_data) + _copacity;
      }
      _item = reinterpret_cast<T*>(_owner->_data) + prev_index;
      return *this;
    }

    T* operator->() const {
      return _item;
    }

    T& operator*() const {
      return *_item;
    }

    friend bool operator==(const iterator& lhs, const iterator& rhs) {
      return lhs._item == rhs._item;
    }

    friend bool operator!=(const iterator& lhs, const iterator& rhs) {
      return !(lhs == rhs);
    }
  };

  static_array() : static_array(sz) { };

  explicit static_array(size_t copacity) : _copacity(copacity), _current_size(0) {
    _data = new char[copacity * sizeof(T)];
    _initialized.resize(copacity);
  }

  ~static_array() {
    clear();
    delete [] _data;
  }

  size_t current_size() {
    return _current_size;
  }

  size_t size() {
    return _copacity;
  }

  void clear() {
    for (auto i = begin(); i != end(); ++i)
      erase(i);
  }

  static_array::iterator emplace(size_t ind, T &&obj) {
    _check_bounds(ind);
    if (_initialized[ind]) {
      (reinterpret_cast<T*>(_data) + ind)->~T();
      --_current_size;
    }
    new (_data + sizeof(T) * ind) T(std::forward<T>(obj));
    _initialized[ind] = true;
    ++_current_size;
    return static_array::iterator(reinterpret_cast<T*>(_data) + ind, this);
  }

  template<class... Args>
  static_array::iterator emplace(size_t ind, Args &&... args) {
    _check_bounds(ind);
    if (_initialized[ind]) {
      reinterpret_cast<T*>(_data + sizeof(T) * ind)->~T();
      --_current_size;
    }
    new (_data + sizeof(T) * ind) T(std::forward<Args>(args)...);
    _initialized[ind] = true;
    ++_current_size;
    return static_array::iterator(reinterpret_cast<T*>(_data) + ind, this);
  }

  void erase(static_array::iterator iter) {
    iter->~T();
    --_current_size;

  }

  T& at(size_t ind) {
    _check_bounds(ind);
    _check_is_initialized(ind);
    return *(reinterpret_cast<T*>(_data) + ind);
  }

  static_array::iterator begin() {
    return static_array::iterator(
             reinterpret_cast<T*>(_data) + _get_next_avaliable_index(-1),
             this
           );
  }

  static_array::iterator end() {
    return static_array::iterator(reinterpret_cast<T*>(_data) + _copacity, this);
  }

 protected:
  char* _data;
  std::vector<bool> _initialized;
  size_t _copacity;
  size_t _current_size;

  void _check_bounds(size_t& ind) {
    if (ind >= _copacity)
      throw std::out_of_range("static_array: Index out of range");
  }

  void _check_is_initialized(size_t& ind) {
    if (!_initialized[ind])
      throw std::runtime_error("static_array: Value is not initialized");
  }

  int _get_next_avaliable_index(int index) {
    for (++index; index < _copacity; ++index)
      if (_initialized[index])
        break;
    return index;
  }

  int _get_prev_avaliable_index(int index) {
    for (--index; index >= 0; --index)
      if (_initialized[index])
        break;
    return index;
  }
};

#endif //HOMEWORK_2_STATIC_ARRAY_H
