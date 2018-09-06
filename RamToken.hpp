#include <eosiolib/currency.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/types.hpp>

namespace eosio {
class RamToken : public eosio::contract {
 private:
  account_name _contract;

 public:
  RamToken(account_name self) : _contract(self), eosio::contract(self) {}
  void apply(account_name contract, account_name act);

};
}  // namespace eosio
