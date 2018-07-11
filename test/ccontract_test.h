#include <cxxtest/TestSuite.h>

#include "crypto/sha256.h"
#include "inc/essential.h"
#include "store/ccontract.h"
#include "store/context_map.h"
#include "util/equation.h"

using eq_ptr = std::unique_ptr<Eq>;

class TempVars : public Vars
{
public:
  uint256_t get_value(Variable var) const final
  {
    switch (var) {
      case Variable::Supply:
        return s;
      case Variable::BNDUSD:
        return 100;
      default:
        return 0;
    }
  }

  uint256_t s;
};

class ComunityContractTest : public CxxTest::TestSuite
{
public:
  void testCreateCC()
  {
    ContextMap ctx;

    // Create equation
    TempVars t;

    t.s = 20;

    // x^3 -3x^2 + 2x - 7
    eq_ptr x = std::make_unique<EqVar>(Variable::Supply);
    eq_ptr c = std::make_unique<EqConstant>(3);
    eq_ptr x3 = std::make_unique<EqExp>(std::move(x), std::move(c));

    x = std::make_unique<EqVar>(Variable::Supply);
    c = std::make_unique<EqConstant>(2);
    x = std::make_unique<EqExp>(std::move(x), std::move(c));
    c = std::make_unique<EqConstant>(3);
    x = std::make_unique<EqMul>(std::move(c), std::move(x));
    x3 = std::make_unique<EqSub>(std::move(x3), std::move(x));

    x = std::make_unique<EqVar>(Variable::Supply);
    c = std::make_unique<EqConstant>(2);
    x = std::make_unique<EqMul>(std::move(c), std::move(x));
    x3 = std::make_unique<EqAdd>(std::move(x3), std::move(x));

    c = std::make_unique<EqConstant>(7);
    x3 = std::make_unique<EqSub>(std::move(x3), std::move(c));

    Curve curve(std::move(x3));

    // Generate random tokenKey
    ContractID key = ContractID::rand();
    CommunityContract contract(ctx, key);
    contract.create(curve);
    contract.set_max_supply(120);

    TS_ASSERT_EQUALS(120, contract.get_max_supply());

    TS_ASSERT_EQUALS(6833, curve.apply(t, false));
    TS_ASSERT_EQUALS(6833, contract.apply_equation(20, false));

    t.s = 4;

    TS_ASSERT_EQUALS(17, contract.apply_equation(4, false));

    contract.set_current_supply(27);
    TS_ASSERT_EQUALS(27, contract.get_current_supply());

    // Test with random contract_id
    ContractID ct = ContractID::rand();
    CommunityContract co2(ctx, ct);
    TS_ASSERT_THROWS_ANYTHING(co2.get_max_supply());
  }
};