#include "creator.h"

#include "contract/account.h"
#include "contract/token.h"
#include "contract/voting.h"
#include "crypto/ed25519.h"
#include "store/context.h"
#include "store/global.h"

Creator::Creator()
    : Contract(Address())
{
}

Address Creator::create(Buffer buf)
{
  ContractID contract_id(ContractID::Creator);
  buf >> contract_id;

  Contract* created_contract = nullptr;

  switch (contract_id) {
    case +ContractID::Account: {
      VerifyKey verify_key = buf.read<VerifyKey>();
      created_contract = &Global::get().m_ctx->create<Account>(verify_key);
      break;
    }
    case +ContractID::Token: {
      Address base = buf.read<Address>();
      Curve buy = buf.read<Curve>();
      created_contract = &Global::get().m_ctx->create<Token>(
          ed25519_vk_to_addr(Global::get().tx_hash), base, buy);
      break;
    }
    case +ContractID::Voting: {
      Address token_id = buf.read<Address>();
      created_contract = &Global::get().m_ctx->create<Voting>(
          ed25519_vk_to_addr(Global::get().tx_hash), token_id);
      break;
    }
    default:
      throw Error("Cannot create contract {}", contract_id._to_string());
  }

  return created_contract->m_addr;
}
