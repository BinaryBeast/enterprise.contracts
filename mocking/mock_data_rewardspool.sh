echo "== EOS MOCK - Data - Rewards Pool =="

echo "--------"
echo "Setting up rewards pool contract"

contract="entrewpool11"
#url="http://localhost:8888"
url="https://api-test.telosfoundation.io"

echo "Set code permission on token contract"
cleos --url $url set account permission --add-code $contract active

cleos --url $url push action $contract reset '[]' -p $contract

echo "Adding action types"
cleos --url $url push action $contract createacttyp '["Create Match", "10.0000 ENT", 23]' -p $contract
cleos --url $url push action $contract createacttyp '["Validate Match", "16.0000 ENT", 10]' -p $contract
cleos --url $url push action $contract createacttyp '["View Match", "5.0000 ENT", 5]' -p $contract