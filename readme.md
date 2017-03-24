## Description
- Hacking the TypeX (British version of Enigma).
- MysteryTwister Level 2 challenge [link](https://www.mysterytwisterc3.org/en/challenges/level-ii/typex-part-1).

## Objective
- Finding the symmetric key by using known-plaintext-attack.

## Requirement
- GCC4.8
- Python3
```
$> brew install gcc48
#> brew install python3
```


## Usage
- Compile
```
$> make
```

- Encryp/decryp
```
$> make runTypex
```

- Verify the key
```
$> make runVerify
```

- Attack
  1. Find cycles
  2. Attack rotors without stators
```
$> make runCycle
$> make runAttack
```
