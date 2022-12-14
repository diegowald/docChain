# docChain
Attempt to add block chain concepts to generic data


We are going to have a way to sign a payload, in a way that can be later checked if it was modified.

The idea is to have a table like the following:

Id, User, payload Checksum, signature

signature is calculated as follows: signature[i] = SHA256(signature[i-1] concat payload)

