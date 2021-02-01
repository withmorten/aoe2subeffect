# aoe2subeffect
cmd tool to make an aoe2 effect command value negative (for UserPatch and DE2)

## Cloning this repository

This repository uses git submodules, so clone it like this:

```
git clone --recurse-submodules <url>
```

## Compiling

You will need cmake >3.0.0 and (some development dependencies).

```
cd code
mkdir build
cd build
cmake ..
cmake --build .
```

You should get a `aoe2subeffect` executable in the `build` folder.

## Using

Prepare a dat file with an effect and command, of which you want the command value to be negative.

Call the `aoe2subeffect` executable:

```
aoe2subeffect <dat> <up/de2> <effect id> <command id>
```

`<dat>` should be the .dat file you want to modify. A backup will be created.

specifiy `up` if you want to to modify an AoC 1.0c dat, de2 if you want to modify a DE2 dat.

`<effet id>` should be the ID of the effect you want to modify, `<command id>` the command ID within that effect.

It will turn the value that command applies negative.
