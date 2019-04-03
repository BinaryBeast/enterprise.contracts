#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
//#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>

#define DEBUG

using namespace eosio;
using std::string;

namespace enterprise {
   class [[eosio::contract]] rewardspool : public contract {
      public:
         rewardspool(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds), current_state(_self, _self.value) { }

         [[eosio::action]]
         void addaccaction(name account) {
            action_accounts act_account(_self, _self.value);
            auto existing = act_account.find(account.value);

            if (existing == act_account.end()) {
               act_account.emplace(_self, [&](auto& aa) {
                  aa.account = account;
                  aa.payable_actions = 1;
               });
               increment_payable_accounts();
               increment_payable_actions();
            }
            else {
               const auto& exaa = *existing;
               act_account.modify(exaa, _self, [&](auto& aa) {
                  aa.payable_actions = ++aa.payable_actions;
               });
               increment_payable_actions();
            }
         }

         [[eosio::action]]
         void setpayables(unsigned long payable_accounts, unsigned long payable_actions) {
            auto c_state = current_state.get_or_create(_self, state { 0, 0 });;

            c_state.payable_accounts = payable_accounts;
            c_state.payable_actions = payable_actions;

            current_state.set(c_state, _self);
         }

         [[eosio::action]]
         void incpayables() {
            increment_payable_actions();
         }

         [[eosio::action]]
         void clearstate() {
            current_state.remove();
         }

      private:
         struct [[eosio::table]] state {
            unsigned long payable_accounts;
            unsigned long payable_actions;
         };
         typedef eosio::singleton<name("state"), state> s_state;
         s_state current_state;

         struct [[eosio::table]] action_account {
            name account;
            int payable_actions;

            uint64_t primary_key() const { return account.value; }
         };
         typedef eosio::multi_index<name("actionaccs"), action_account> action_accounts;

         void increment_payable_actions() {
            auto c_state = current_state.get_or_create(_self, state { 0, 0 });

            c_state.payable_actions = c_state.payable_actions + 1;

            current_state.set(c_state, _self);
         }

         void increment_payable_accounts() {
            auto c_state = current_state.get_or_create(_self, state { 0, 0 });

            c_state.payable_accounts = c_state.payable_accounts + 1;

            current_state.set(c_state, _self);
         }
   };
}