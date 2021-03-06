#ifndef __BIT_STREAM_H__
#define __BIT_STREAM_H__

#include <string.h>

#include "common_define.h"

class BitStream {
 public:
  BitStream() {
    buf_ = fixed_size_buf_;
    buf_size_ = sizeof(fixed_size_buf_);
    bzero(buf_, buf_size_);
    bit_len_ = buf_size_ * 8;
    cur_pos_ = 0;
  }

  BitStream(uint8_t* buf, size_t buf_size) {
    buf_ = buf;
    buf_size_ = buf_size;
    bzero(buf_, buf_size_);
    bit_len_ = buf_size_ * 8;
    cur_pos_ = 0;
  }

  template <typename T>
  int WriteBits(const size_t& bits, const T& val) {
    if (cur_pos_ + bits > bit_len_) {
      std::cout << LMSG << "write " << bits << " bits will be overflow"
                << std::endl;
      return -1;
    }

    T mask = 1UL << (bits - 1);

    for (size_t i = 0; i != bits; ++i) {
      if (val & mask) {
        buf_[cur_pos_ / 8] |= (1 << (7 - (cur_pos_ % 8)));
      }

      mask >>= 1;
      ++cur_pos_;
    }
    return 0;
  }

  template <typename T>
  int ReplaceBytes(const int& pos, const size_t& bytes, const T& val) {
    if (pos > (int)cur_pos_ / 8) {
      std::cout << LMSG << "replace in " << pos
                << " overflow, cur_pos:" << (cur_pos_ / 8) << std::endl;
      return -1;
    }

    const uint8_t* p = (const uint8_t*)&val;

    for (size_t i = 0; i != bytes; ++i) {
      buf_[pos + i] = p[bytes - 1 - i];
    }

    return 0;
  }

  template <typename T>
  int ModifyBytes(const uint32_t pos, const size_t& bytes, const T& val) {
    if (pos >= cur_pos_ / 8) {
      return -1;
    }

    const uint8_t* p = (const uint8_t*)&val;

    for (size_t i = 0; i != bytes; ++i) {
      buf_[pos + i] = p[bytes - 1 - i];
    }

    return 0;
  }

  template <typename T>
  int WriteBytes(const size_t& bytes, const T& val) {
    const uint8_t* p = (const uint8_t*)&val;

    for (size_t i = 0; i != bytes; ++i) {
      buf_[cur_pos_ / 8 + i] = p[bytes - 1 - i];
    }

    cur_pos_ += bytes * 8;

    return 0;
  }

  int WriteData(const size_t& bytes, const uint8_t* data) {
    memcpy(buf_ + cur_pos_ / 8, data, bytes);
    cur_pos_ += bytes * 8;

    return 0;
  }

  uint32_t SizeInBytes() { return cur_pos_ / 8; }

  uint8_t* GetData() { return buf_; }

 private:
  uint8_t* buf_;
  size_t buf_size_;
  uint8_t fixed_size_buf_[1024 * 16];
  uint32_t bit_len_;
  uint32_t cur_pos_;
};

#endif  // __BIT_STREAM_H__
