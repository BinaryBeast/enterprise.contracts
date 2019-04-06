#include <rewardspool.hpp>

ACTION rewardspool::addaccaction(name account) {
  action_accounts act_account(_self, _self.value);
  auto existing = act_account.find(account.value);
  print("Action added for:  ", name(account), "\n");

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

ACTION rewardspool::setpayables(unsigned long payable_accounts, unsigned long payable_actions) {
  auto c_state = current_state.get_or_create(_self, state { 0, 0 });;

  c_state.payable_accounts = payable_accounts;
  c_state.payable_actions = payable_actions;

  current_state.set(c_state, _self);
}

ACTION rewardspool::incpayables() {
  increment_payable_actions();
}

ACTION rewardspool::clearstate() {
  current_state.remove();
}

ACTION rewardspool::payrewards(asset inflation_asset) {
  pay_rewards(inflation_asset);
}

void rewardspool::inflation (name to, asset quantity) {
  auto self = get_self();
  print("Inflation issued to ", name(to), " totalling ", quantity.to_string(), "\n");

  if (to == self) {
    print("This is my inflation\n");
    pay_rewards(quantity);
  }
}

void rewardspool::increment_payable_actions() {
  auto c_state = current_state.get_or_create(_self, state { 0, 0 });

  c_state.payable_actions = c_state.payable_actions + 1;

  current_state.set(c_state, _self);
}

void rewardspool::increment_payable_accounts() {
  auto c_state = current_state.get_or_create(_self, state { 0, 0 });

  c_state.payable_accounts = c_state.payable_accounts + 1;

  current_state.set(c_state, _self);
}

void rewardspool::pay_rewards(asset inflation_asset) {
  action_accounts act_accounts(_self, _self.value);

  auto c_state = current_state.get();
  auto rewards_per_action = inflation_asset.amount / c_state.payable_actions;
  print("Rewards Per Action == ", rewards_per_action, "\n");

  for (auto &act_account : act_accounts) {
     auto reward_amount = rewards_per_action * act_account.payable_actions;
     asset reward_asset(reward_amount, inflation_asset.symbol);
     print("Paying Reward (", reward_asset.to_string(), ") to account ", name(act_account.account), "\n");

     token::transfer_action transfer(name("gre111111111"), {get_self(), name("active")});
     transfer.send(_self, act_account.account, reward_asset, "Rewards");
  }
}