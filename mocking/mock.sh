echo "== EOS MOCK =="

walletName="eosmock"

echo "Creating wallet..."
cleos wallet create --name $walletName --to-console #| tail -1 | awk -F '"' '{pass="$2"}' | echo "Wallet Created | Name: $walletName | PW: $pass"