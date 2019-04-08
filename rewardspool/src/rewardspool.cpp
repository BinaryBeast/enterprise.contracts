#include <rewardspool.hpp>

ACTION rewardspool::createacttyp(string type, asset max_reward, unsigned int max_pay_outs) {
  rewards_action_types actions_types(_self, _self.value);
  
  actions_types.emplace(_self, [&](auto& at) {
    at.id = actions_types.available_primary_key();
    at.type = type;
    at.max_reward = max_reward;
    at.max_pay_outs = max_pay_outs;
  });
}

ACTION rewardspool::createact(name source, name owner, uint64_t type_id) {
  rewards_action_types actions_types(_self, _self.value);
  auto action_type = actions_types.find(type_id);
  check(action_type != actions_types.end(), "Action type does not exist");
  const auto& at = *action_type;

  rewards_actions actions(_self, type_id);
  
  actions.emplace(_self, [&](auto& a) {
    a.id = actions.available_primary_key();
    a.source = source;
    a.owner = owner;
    a.current_pay_outs = 0;
    a.rewards_paid = asset(0, at.max_reward.symbol);
  });
  
  increment_payable_actions();
}

void rewardspool::inflation (name to, asset quantity) {
  auto self = get_self();

  if (to == self) {
    print("Paying inflation from ", name(to), " totalling ", quantity.to_string(), "\n");
    pay_rewards(quantity);
  }
}

void rewardspool::increment_payable_actions() {
  auto c_state = current_state.get_or_create(_self, state { 0 });

  c_state.payable_actions = c_state.payable_actions + 1;

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
  
// Deprecated / Testing
ACTION rewardspool::setpayables(unsigned long payable_actions) {
  auto c_state = current_state.get_or_create(_self, state { 0 });;

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