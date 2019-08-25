echo "== EOS MOCK - Data - Token =="

echo "--------"
echo "Setting up token contract"

contract="token1111111"

echo "Set code permission on token contract"
cleos set account permission --add-code $contract active

echo "Create ENT token"
cleos push action $contract create '[ "token1111111", "1000000000.0000 ENT"]' -p $contract

echo "Issue ENT tokens"
cleos push action $contract issue '[ "ta1111111111", "100.0000 ENT", "Initial test issue of token" ]' -p $contract
cleos push action $contract issue '[ "ta2111111111", "100.0000 ENT", "Initial test issue of token" ]' -p $contract

echo "Set inflation pools"
cleos push action $contract setinflpools '{ "pools":[ "rewardspool1", "pool21111111", "pool31111111" ], "distribution_percentages":[ 30, 60, 10 ] }' -p $contract

echo "Set inflation"
cleos push action $contract setinflation '[ "0.0000 ENT", "0.00025", "600", true ]' -p $contract

echo "Start inflation"
cleos push action $contract stainflation '[ "0.0000 ENT" ]' -p $contract