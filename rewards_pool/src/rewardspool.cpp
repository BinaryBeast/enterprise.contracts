#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/singleton.hpp>

#define DEBUG

using namespace eosio;
using std::string;

namespace enterprise {
   class [[eosio::contract]] rewardspool : public contract {
      public:
         rewardspool(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds), state(_self, _self.value) { }

         [[eosio::action]]
         void addaccaction(name account) {
            action_accounts act_account(_self, _self.value);
            auto existing = act_account.find(account.value);

            if (existing == act_account.end()) {
               const auto& exaa = *existing;
               act_account.modify(exaa, _self, [&](auto& aa) {
                  aa.payable_actions = aa.payable_actions++;
               });
            }
            else {
               act_account.emplace(_self, [&](auto& aa) {
                  aa.account = account;
                  aa.payable_actions = 1;
               });
               auto current_state = state.get();

               current_state.payable_accounts = current_state.payable_accounts++;

               state.set(current_state, _self);
            }
         }

      private:
         struct [[eosio::table]] state {
            unsigned long payable_accounts;
         };
         typedef eosio::singleton<"state"_n, state> s_state;
         s_state state;

         struct [[eosio::table]] action_account {
            name account;
            int payable_actions;

            uint64_t primary_key() const { return account.value; }
         };
         typedef eosio::multi_index<"actionaccs"_n, action_account> action_accounts;
   };
}

EOSIO_DISPATCH(enterprise::rewardspool, (addaccaction))