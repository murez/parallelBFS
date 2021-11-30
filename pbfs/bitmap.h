#ifndef BITMAP_H_
#define BITMAP_H_

#include <algorithm>
#include <cstdint>
const uint64_t BITPERWORD = 64;
class Bitmap {
public:
    Bitmap(int size) {
        uint64_t num_words = (size + BITPERWORD - 1) / BITPERWORD;
        start_ = new uint64_t[num_words];
        end_ = start_ + num_words;
    }

    ~Bitmap() { delete[] start_; }

    void reset() { std::fill(start_, end_, 0); }

    uint64_t arrIdx(int n) { return n / BITPERWORD; }

    uint64_t bitIdx(int n) { return n & (BITPERWORD - 1); }

    bool isSet(int n) { return start_[arrIdx(n)] & (1l << bitIdx(n)); }

    void setBit(int n) { start_[arrIdx(n)] |= ((uint64_t) 1l << bitIdx(n)); }

    void setBitAtomic(int n) {
        uint64_t curVal, newVal;
        do {
            curVal = start_[arrIdx(n)];
            newVal = curVal | ((uint64_t) 1l << bitIdx(n));
        } while (!__sync_bool_compare_and_swap(&start_[arrIdx(n)], curVal, newVal));
    }

    void swap(Bitmap &inputstart_) {
        std::swap(start_, inputstart_.start_);
        std::swap(end_, inputstart_.end_);
    }

private:

    uint64_t *start_;
    uint64_t *end_;
};

#endif
