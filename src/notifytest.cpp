#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

class [[eosio::contract]] notifytest : public contract {
   public:
      using contract::contract;

      [[eosio::action]] 
      void addstat(name account, std::string action_text) {
          print("Adding Stat: ", name(account), " Action: ", action_text, "\n");
          add_stat(account, action_text);
      }

      [[eosio::on_notify("eosio.token::transfer")]] 
      void on_transfer(name from, name to, asset quant, std::string memo) {
         check(_first_receiver == name("eosio.token"), "should be eosio.token");
         print_f("On notify : % % % %\n", from, to, quant, memo);
         add_stat(to, "eosio.token transfer");
      }

      [[eosio::on_notify("gre111111111::issinflation")]] 
      void on_issinflation(name to, asset quantity) {
         check(_first_receiver == name("gre111111111"), "should be gre111111111");
         print("Issue Inflation ", name(to), " Amount ", quantity.to_string(), "\n");
         add_stat(to, "Issue Inflation");
      }

    private:
        struct [[eosio::table]] notifystat {
            name account;
            std::string last_action;
            int count;

            uint64_t primary_key() const { return account.value; }
        };
        typedef eosio::multi_index<name("notifystats"), notifystat> notifystats;

        void add_stat(name account, std::string action_text) {
            notifystats stats(_self, _self.value);
            auto existing = stats.find(account.value);
            print("Action added for:  ", name(account), "\n");

            if (existing == stats.end()) {
               stats.emplace(_self, [&](auto& aa) {
                  aa.account = account;
                  aa.last_action = action_text;
                  aa.count = 1;
               });
            }
            else {
               const auto& exaa = *existing;
               stats.modify(exaa, _self, [&](auto& aa) {
                    aa.last_action = action_text;
                    aa.count = ++aa.count;
               });
            }
        }
};