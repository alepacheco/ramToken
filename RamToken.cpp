#include "ramToken.hpp"

namespace eosio {
void RamToken::apply(account_name contract, account_name act) {
  switch (act) {
    case N(transfer): {
      transfer_t data = unpack_action_data<transfer_t>();
      // If deposit
      if (contract == N(eosio.token)) {
        if (data.from == _self || data.from == N(eosio.ram) ||
            data.from == N(eosio) || data.to == N(eosio.ramfee) ||
            data.to == N(eosio.ram)) {
          break;
        }

        require_auth(data.from);
        int64_t bytesBought = buyRam(data.quantity, _self);
        print(" bought: ", bytesBought);

        add_balance(data.from, asset(bytesBought, SYMBOL), _self);
      }
      // If transfer
      if (contract == _self) {
        transfer(data.from, data.to, data.quantity, data.memo);
      }
      break;
    }

    case N(claim): {
      claim_t data = unpack_action_data<claim_t>();
      require_auth(data.user);
      sub_balance(data.user, data.quantity);
      auto eosReceived = sellRam(data.quantity);
      action(permission_level(_self, N(active)), N(eosio.token), N(transfer),
             std::make_tuple(_self, data.user, eosReceived,
                             std::string("ram claim")))
          .send();
      break;
    }
    case N(create): {
      create();
      break;
    }
  }
}

void RamToken::create() {
  require_auth(_self);
  stats statstable(_self, SYMBOL.name());
  auto existing = statstable.find(SYMBOL.name());
  eosio_assert(existing == statstable.end(),
              "token with symbol already exists");

  statstable.emplace(_self, [&](auto& s) {
    s.supply = asset(0, SYMBOL);
    s.max_supply = asset(pow(2, 61) - 1, SYMBOL);
    s.issuer = _self;
  });
}

int64_t RamToken::buyRam(asset quantity /* EOS */, account_name user) {
  eosio_assert(quantity.amount > 0, "must purchase a positive amount");
  auto fee = quantity;
  fee.amount = (fee.amount + 199) / 200;
  auto quant_after_fee = quantity;
  quant_after_fee.amount -= fee.amount;

  auto itr = rammarket(N(eosio), N(eosio)).find(S(4, RAMCORE));
  auto tmp = *itr;
  asset bytes = tmp.convert(quant_after_fee, S(0, RAM));
  action(permission_level(_self, N(active)), N(eosio), N(buyram),
         std::make_tuple(_self, user, quantity))
      .send();

  return bytes.amount;
}

asset RamToken::sellRam(asset quantity) {
  eosio_assert(quantity.amount > 0, "must purchase a positive amount");
  eosio_assert(quantity.symbol == SYMBOL, "must use ram symbol");

  auto itr = rammarket(N(eosio), N(eosio)).find(S(4, RAMCORE));
  auto tmp = *itr;
  auto tokens_out = tmp.convert(asset(quantity.amount, S(0,RAM)), CORE_SYMBOL);
  auto fee = (tokens_out.amount + 199) / 200;
  tokens_out.amount -= fee;

  action(permission_level(_self, N(active)), N(eosio), N(sellram),
         std::make_tuple(_self, quantity))
      .send();

  return tokens_out;
}

void RamToken::transfer(account_name from, account_name to, asset quantity,
                        string memo) {
  eosio_assert(from != to, "cannot transfer to self");
  require_auth(from);
  eosio_assert(is_account(to), "to account does not exist");

  require_recipient(from);
  require_recipient(to);

  eosio_assert(quantity.is_valid(), "invalid quantity");
  eosio_assert(quantity.amount > 0, "must transfer positive quantity");
  eosio_assert(quantity.symbol == SYMBOL, "symbol precision mismatch");
  eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");
  auto payer = has_auth(to) ? to : from;
  sub_balance(from, quantity);
  add_balance(to, quantity, payer);
}

void RamToken::sub_balance(account_name owner, asset value) {
  stats statstable(_self, value.symbol.name());
  auto existing = statstable.find(value.symbol.name());
  eosio_assert(existing != statstable.end(),
               "token with symbol does not exist, create token before issue");
  statstable.modify(existing, 0, [&](auto& s) { s.supply -= value; });

  accounts from_acnts(_self, owner);
  const auto& from =
      from_acnts.get(value.symbol.name(), "no balance object found");
  eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

  from_acnts.modify(from, owner, [&](auto& a) { a.balance -= value; });
}

void RamToken::add_balance(account_name owner, asset value,
                           account_name ram_payer) {
  stats statstable(_self, value.symbol.name());
  auto existing = statstable.find(value.symbol.name());
  eosio_assert(existing != statstable.end(),
               "token with symbol does not exist, create token before issue");
  statstable.modify(existing, 0, [&](auto& s) { s.supply += value; });

  accounts to_acnts(_self, owner);
  auto to = to_acnts.find(value.symbol.name());
  if (to == to_acnts.end()) {
    to_acnts.emplace(ram_payer, [&](auto& a) { a.balance = value; });
  } else {
    to_acnts.modify(to, 0, [&](auto& a) { a.balance += value; });
  }
}

}  // namespace eosio

extern "C" {
[[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  eosio::RamToken ct(receiver);
  ct.apply(code, action);
  eosio_exit(0);
}
}