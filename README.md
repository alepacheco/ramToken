# RAM Token contract
Token for the EOS network backed and redeemable

## Actions

* transfer: 
Used to send tokens to other users, same as any other token

* claim: Used to get the EOS value of your ram tokens

## Usage

To get RAM tokens send EOS to the contract address. Once you have RAM tokens you can used it as any other token. The price for this token will be similar or identical to the ram price, if you want to get the EOS value for your ram tokens use the _claim_ action

## Examples

- Get RAM: `cleos transfer alepachecopp ramtoken1234 "1000.0000 EOS" ""`

- Send RAM: `cleos push action ramtoken1234 transfer '{"from": "alepachecopp", "to": "b1", "quantity": "1.0000 RAM", "memo": "Hi"}' -p alepachecopp`

- Claim RAM: `cleos push action ramtoken1234 claim '{"user": "alepachecopp", "quantity": "16.6162 RAM", "memo": ""}' -p alepachecopp`
