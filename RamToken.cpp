#include "RamToken.hpp"
#include <cmath>
#include <eosiolib/currency.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/types.hpp>

namespace eosio {
void RamToken::apply(account_name contract, account_name act) {
  switch (act) {
    case N(transfer): {
      break;
    }
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