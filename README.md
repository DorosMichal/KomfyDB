# KomfyDB

Simple database C++ implemenatation based on MIT SimpleDB.

## Requierments

- clang-13
- bazel
- openjdk-11-jdk

On debian-based:
```
sudo apt update && sudo apt install clang-13 bazel openjdk-11-jdk
```

Also, add this to your `.bashrc`:
```
export CC=clang-13
```

Or, everytime you issue a bazel command, add this as a prefix:
```
CC=clang-13 bazel ...
```
