echo "== EOS MOCK - Accounts =="

walletName="eosmock"

# WARNING: The keys are for development purposes only and should not be used in production
privateKeys=(
    "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"   # EOSIO DEV Private Key (eosio contract)
    "5JCxNpx1DNyDrXA8xBQVie2VD5s6afEzaM4zEjVXEnHdFR9uMTb"   # Token (contract)
    "5JWjXb1S8qtLCYTKiTb1kT2E4WMkLTMtKagPJt3eWi1xZUfYFZY"   # Rewards Pool (contract)
    "5KaWrXLvHR9oU9AbS5gjxRqbk8ZqPHvTgRwDdeWbvMnoEpuZrMb"   # Pool 2
    "5KjjnZEvAnrQY274nuQRi6P2G2L6ipi9wv6itCSVtiXufobinoz"   # Pool 3
    "5JWXLHYfsV1c4fGPv8f8hYXDMbwnZyQazgmyvWh5gNK5yGEuXST"   # Match (contract)
    "5HqSrecwEqXYzv8QDc9EwkNMWZfBftnHKN4NhNNr9GPjfBsUsSu"   # Test Account 1
    "5JhLLRU9P91kmw64yY9dxNHEZL5vXtp1DWy22svXu1F2oFvXta4"   # Test Account 2
)

echo "Importing private keys..."
for pk in "${privateKeys[@]}"
do
    cleos wallet import --name $walletName --private-key $pk
done

declare -A accounts=(
    [enttoken1111]="EOS5L4TGAKjUPhLACM6ZXpYm6aRQFLPko2zGGaCqhbcGRCDw6FH5j"
    [entrewpool11]="EOS7syYK6HbNH3ZRj2tf2uLBQMGXxe2z8KaMTsYuANvDDYZrhicF9"
    [entpool21111]="EOS6t3GW4atHhnKewNGoeUxyRFezXsbD3i81kYRQHrj31KQSLyJUK"
    [entpool31111]="EOS6CKhVNtnK2n6VbeqBPV9CG5kZhrZuhqLmjmsKLudNMUFzDUwWc"
    [entmatching1]="EOS7j6tvbUJu6bv4QpeVMWrGLjPESBNuoAKh9exX73Qb2DHLQUXP9"
    [entta1111111]="EOS7yYcGbp6eRQWuhTG6CDBiUngFQahBFxzMUQmRFAguQhtTVpSsy"
    [entta2111111]="EOS61GUXZvmKVzVDnqDJ3LwVnVzF8ZjWKLivwMDb7BLRsYYMMFA8H"
)

echo "Creating accounts..."
for account in "${!accounts[@]}"
do
    cleos push action eosio newaccount '{
        "creator":"eosio",
        "name":"'$account'",
        "owner":{"accounts":[],"keys":[{"key":"'${accounts[$account]}'","weight":1}],"threshold":1,"waits":[]},
        "active":{"accounts":[],"keys":[{"key":"'${accounts[$account]}'","weight":1}],"threshold":1,"waits":[]}
    }' -p eosio@active
    echo "Created account $account"
done