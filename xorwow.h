#ifndef _XORWOW_H
#define _XORWOW_H

#include <cstdint>

struct xorwow_state {
  uint32_t a, b, c, d;
  uint32_t counter;
};

uint64_t round_down(uint64_t n, uint64_t m);
uint64_t round_up(uint64_t n, uint64_t m);
uint64_t calc_num_kmers(uint64_t l, uint8_t k);

void xorwow_init(xorwow_state *);
uint32_t xorwow(xorwow_state *);

class XorwowDist {
public:

  XorwowDist() {
    xorwow_init(&state_);
  }

  std::uint64_t operator()() { return xorwow(&state_); }

private:
  xorwow_state state_;
};

#endif  //_XORWOW_H
