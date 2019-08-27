echo "== EOS MOCK - Data - Matching =="

echo "--------"
echo "Setting up matching contract"

contract="matching1111"
testAccount1="ta1111111111"
testAccount2="ta2111111111"

echo "Adding match data"

cleos push action $contract reset '[]' -p $contract

cleos push action $contract mchtypcreate '["a820ea4f8b60e1599f8aa1ff3aba9db2466738ffcf983e6609e7ab709575d0bb", "Head 2 Head", 2]' -p $contract
cleos push action $contract mchtypcreate '["06d718ca948754a2b0a6f460630c8c50a1f9488a754a3a0165658d24f329a3ed", "Battle Royale", 100]' -p $contract
cleos push action $contract mchtypcreate '["a3315824bcdc7bd6e86d2744f3f7f5d7207147c840f9195705b6b8e163b27a6f", "Four Way", 4]' -p $contract

cleos push action $contract mchcreate '["7af12386a82b6337d6b1e4c6a1119e29bb03e6209aa03c70ed3efbb9b74a290c", "a820ea4f8b60e1599f8aa1ff3aba9db2466738ffcf983e6609e7ab709575d0bb", "Enterprise Crew Playoff", '"$testAccount1"', "2019-05-26T15:00:00.000"]' -p $testAccount1
cleos push action $contract mchsetstatus '["7af12386a82b6337d6b1e4c6a1119e29bb03e6209aa03c70ed3efbb9b74a290c", 1]' -p $testAccount1
cleos push action $contract mchcreateopp '["Riker", "7af12386a82b6337d6b1e4c6a1119e29bb03e6209aa03c70ed3efbb9b74a290c"]' -p $testAccount1
cleos push action $contract mchcreateopp '["Data", "7af12386a82b6337d6b1e4c6a1119e29bb03e6209aa03c70ed3efbb9b74a290c"]' -p $testAccount1
cleos push action $contract mchcreate '["69f5f38b3aade18a48dd7edc11277a35c39cec36c11eb4342973969265a2f3a3", "a820ea4f8b60e1599f8aa1ff3aba9db2466738ffcf983e6609e7ab709575d0bb", "Voyager Crew Playoff", '"$testAccount1"', "2019-05-27T15:00:00.000"]' -p $testAccount1
cleos push action $contract mchsetstatus '["69f5f38b3aade18a48dd7edc11277a35c39cec36c11eb4342973969265a2f3a3", 2]' -p $testAccount1
cleos push action $contract mchcreateopp '["Neelix", "69f5f38b3aade18a48dd7edc11277a35c39cec36c11eb4342973969265a2f3a3"]' -p $testAccount1
cleos push action $contract mchcreateopp '["Kes", "69f5f38b3aade18a48dd7edc11277a35c39cec36c11eb4342973969265a2f3a3"]' -p $testAccount1
cleos push action $contract mchcreate '["e2ead1f6c5aa8f79df572cf6dbae214fa69c6787de44d789f937b5c472cd2397", "06d718ca948754a2b0a6f460630c8c50a1f9488a754a3a0165658d24f329a3ed", "Voyager Crew Royale", '"$testAccount1"', "2019-05-27T19:00:00.000"]' -p $testAccount1
cleos push action $contract mchcreateopp '["Janeway", "e2ead1f6c5aa8f79df572cf6dbae214fa69c6787de44d789f937b5c472cd2397"]' -p $testAccount1
cleos push action $contract mchcreateopp '["Chakotay", "e2ead1f6c5aa8f79df572cf6dbae214fa69c6787de44d789f937b5c472cd2397"]' -p $testAccount1
cleos push action $contract mchcreateopp '["Seven of Nine", "e2ead1f6c5aa8f79df572cf6dbae214fa69c6787de44d789f937b5c472cd2397"]' -p $testAccount1
cleos push action $contract mchcreateopp '["Kim", "e2ead1f6c5aa8f79df572cf6dbae214fa69c6787de44d789f937b5c472cd2397"]' -p $testAccount1
cleos push action $contract mchcreateopp '["Paris", "e2ead1f6c5aa8f79df572cf6dbae214fa69c6787de44d789f937b5c472cd2397"]' -p $testAccount1
cleos push action $contract mchcreateopp '["Torres", "e2ead1f6c5aa8f79df572cf6dbae214fa69c6787de44d789f937b5c472cd2397"]' -p $testAccount1
cleos push action $contract mchcreate '["6651949046721aa674b597769866454501ea16d5418391f97d03be5cedccc806", "a820ea4f8b60e1599f8aa1ff3aba9db2466738ffcf983e6609e7ab709575d0bb", "Enterprise Crew Playoff 2", '"$testAccount1"', "2019-05-27T19:00:00.000"]' -p $testAccount1
cleos push action $contract mchsetstatus '["6651949046721aa674b597769866454501ea16d5418391f97d03be5cedccc806", 1]' -p $testAccount1
cleos push action $contract mchcreateopp '["Picard", "6651949046721aa674b597769866454501ea16d5418391f97d03be5cedccc806"]' -p $testAccount1
cleos push action $contract mchcreateopp '["Troi", "6651949046721aa674b597769866454501ea16d5418391f97d03be5cedccc806"]' -p $testAccount1