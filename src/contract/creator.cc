#include "creator.h"

#include "contract/account.h"
#include "contract/governance.h"
#include "contract/stake.h"
#include "contract/tcr.h"
#include "contract/token.h"
#include "contract/voting.h"
#include "crypto/ed25519.h"
#include "store/context.h"
#include "store/global.h"

Creator::Creator(const Address& address)
    : Contract(address, ContractID::Creator)
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
      auto& account =
          Global::get().m_ctx->create<Account>(ed25519_vk_to_addr(verify_key));
      account.init(verify_key);
      created_contract = &account;
      break;
    }
    case +ContractID::Token: {
      Address base = buf.read<Address>();
      Curve buy = buf.read<Curve>();
      auto& token = Global::get().m_ctx->create<Token>(
          ed25519_vk_to_addr(Global::get().tx_hash));
      token.init(base, buy);
      created_contract = &token;
      break;
    }
    // case +ContractID::Voting: {
    //   Address token_id = buf.read<Address>();
    //   created_contract = &Global::get().m_ctx->create<Voting>(
    //       ed25519_vk_to_addr(Global::get().tx_hash), token_id);
    //   break;
    // }

    // case +ContractID::Registry: {
    //   Address token_id = buf.read<Address>();
    //   Address voting_id = buf.read<Address>();
    //   Address governance_id = buf.read<Address>();
    //   uint8_t vote_quorum = buf.read<uint8_t>();
    //   uint8_t dispensation_percentage = buf.read<uint8_t>();
    //   uint256_t min_deposit = buf.read<uint256_t>();
    //   uint64_t apply_duration = buf.read<uint64_t>();
    //   uint64_t commit_duration = buf.read<uint64_t>();
    //   uint64_t reveal_duration = buf.read<uint64_t>();
    //   created_contract = &Global::get().m_ctx->create<Registry>(
    //       ed25519_vk_to_addr(Global::get().tx_hash), token_id, voting_id,
    //       governance_id, vote_quorum, dispensation_percentage, min_deposit,
    //       apply_duration, commit_duration, reveal_duration);
    //   break;
    // }

    // case +ContractID::Stake: {
    //   Address token_id = buf.read<Address>();
    //   created_contract = &Global::get().m_ctx->create<Stake>(
    //       ed25519_vk_to_addr(Global::get().tx_hash), token_id);
    //   break;
    // }

    // case +ContractID::Governance: {
    //   Address token_id = buf.read<Address>();
    //   Address voting_id = buf.read<Address>();
    //   uint8_t losing_threshold = buf.read<uint8_t>();
    //   uint8_t winning_threshold = buf.read<uint8_t>();
    //   uint256_t min_deposit = buf.read<uint256_t>();
    //   uint64_t commit_duration = buf.read<uint64_t>();
    //   uint64_t reveal_duration = buf.read<uint64_t>();
    //   created_contract = &Global::get().m_ctx->create<Governance>(
    //       ed25519_vk_to_addr(Global::get().tx_hash), token_id, voting_id,
    //       losing_threshold, winning_threshold, min_deposit, commit_duration,
    //       reveal_duration);
    //   break;
    // }
    default:
      throw Error("Cannot create contract {}", contract_id._to_string());
  }

  return created_contract->m_addr;
}
