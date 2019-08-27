echo "== EOS MOCK - Data - Rewards Pool =="

echo "--------"
echo "Setting up rewards pool contract"

contract="rewardspool1"

echo "Set code permission on token contract"
cleos set account permission --add-code $contract active

cleos push action $contract reset '[]' -p $contract

echo "Adding action types"
cleos push action $contract createacttyp '["Create Match", "10.0000 ENT", 23]' -p $contract
cleos push action $contract createacttyp '["Validate Match", "16.0000 ENT", 10]' -p $contract
cleos push action $contract createacttyp '["View Match", "5.0000 ENT", 5]' -p $contract