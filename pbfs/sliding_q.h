#ifndef SLIDING_Q_H_
#define SLIDING_Q_H_

#include <algorithm>

template<typename T>
class Qbuffer;

template<typename T>
class SlidingQ {
public:
    SlidingQ(T s) {
        sq = new T[s];
        idx_in = 0, idx_s = 0, idx_e = 0;
    }

    ~SlidingQ() { delete[] sq; }

    void push_back(T val) { sq[idx_in++] = val; }

    void slide() {
        idx_s = idx_e;
        idx_e = idx_in;
    }

    bool empty() { return idx_s == idx_e; }

    T *begin() { return sq + idx_s; }

    T *end() { return sq + idx_e; }

    int size() { return idx_e - idx_s; }

private:
    int idx_in, idx_s, idx_e;
    T *sq;

    friend class Qbuffer<T>;
};

template<typename T>
class Qbuffer {
public:
    Qbuffer(SlidingQ<T> &master, int size = 16384*4) : sq(master), lqSize(size) {
        idx = 0;
        lq = new T[lqSize];
    }

    ~Qbuffer() { delete[] lq; }

    void flush() {
        T *shared = sq.sq;
        int from = __sync_fetch_and_add(&sq.idx_in, idx);
        std::copy(lq, lq + idx, shared + from);
        idx = 0;
    }

    void push_back(int val) {
        if (idx == lqSize) flush();
        lq[idx++] = val;
    }

private:
    int idx;
    T *lq;
    SlidingQ<T> &sq;
    const int lqSize;
};


#endif