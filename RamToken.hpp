#include <eosiolib/currency.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/types.hpp>

#include <cmath>
#include <functional>
#include <string>

#include "ram/exchange_state.cpp"

namespace eosio {
class RamToken : public eosio::contract {
 private:
  account_name _contract;
  const symbol_type SYMBOL = S(4, KB);

  struct transfer_t {
    account_name from;
    account_name to;
    asset quantity;
    string memo;
  };

  struct claim_t {
    account_name user;
    asset quantity;
    string memo;
  };

  //@abi table accounts i64
  struct account {
    asset balance;
    uint64_t primary_key() const { return balance.symbol.name(); }
  };

  //@abi table stat i64
  struct stats_t {
    asset supply;
    account_name issuer;
    uint64_t primary_key() const { return supply.symbol.name(); }
  };

  typedef eosio::multi_index<N(accounts), account> accounts;
  typedef eosio::multi_index<N(stat), stats_t> stats;

  asset buyRam(asset quantity, account_name user);
  asset sellRam(asset quantity);

  void add_balance(account_name owner, asset value, account_name ram_payer);

  void sub_balance(account_name owner, asset value);

 public:
  RamToken(account_name self) : _contract(self), eosio::contract(self) {}
  void apply(account_name contract, account_name act);

  /// @abi action
  void claim(account_name user, asset quantity, string memo);

  /// @abi action
  void transfer(account_name from, account_name to, asset quantity,
                string memo);
};

}  // namespace eosio
