#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "util/variable.h"

class VarsContext : public Vars
{
public:
  VarsContext(Context& _ctx, uint256_t _supply);
  uint256_t get_value(Variable var) const override;
  void set_supply(uint256_t _supply) { supply = _supply; }

protected:
  Context& ctx;
  uint256_t supply;
};

// Variable context for product token contract
class VarsContextPT : public VarsContext
{
public:
  VarsContextPT(Context& _ctx, uint256_t _supply, uint256_t _ct_price);
  uint256_t get_value(Variable var) const final;
  void set_ct_price(uint256_t _ct_price) { ct_price = _ct_price; }

private:
  uint256_t ct_price;
};
