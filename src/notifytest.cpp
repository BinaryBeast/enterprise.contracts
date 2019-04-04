#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/name.hpp>

using namespace eosio;

class [[eosio::contract]] notifytest : public contract {
   public:
      using contract::contract;

      [[eosio::action]] 
      void test1(name nm) {
         check(nm == "bucky"_n, "not bucky");
      }

      [[eosio::on_notify("eosio.token::transfer")]] 
      void on_transfer(name from, name to, asset quant, std::string memo) {
         check(_code == "eosio.token"_n, "should be eosio.token");
         print_f("On notify : % % % %", from, to, quant, memo);
      }
};