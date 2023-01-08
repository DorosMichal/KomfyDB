# KomfyDB

Toy database C++ implemenatation based on MIT's SimpleDB.

## Cloning

Run:
```
git clone --recursive https://github.com/DorosMichal/KomfyDB
```

## Requierments

- clang-13
- bazel
- openjdk-11-jdk

On Debian Bullseye:

```
sudo apt update && sudo apt install clang-13 bazel openjdk-11-jdk
```

## Using bazel

I highly encourage you to take a look on 
[this tutorial](https://docs.bazel.build/versions/main/tutorial/cpp.html) as
it's really short and shows almost everything we need here.

First run:
```
source ./env.sh
```

To see if bazel is working run (anywhere in the source tree!):

```
bazel run //komfydb:komfydb -- --logtostderr=1
```

To cleanup simply run
```
bazel clean --expunge
```

### Debugging

Build the project by running:
```
bazel build --config=debug //...
```

### Profiling

Build the project with these additional options:
```
bazel build --copt=-pg --cxxopt=-pg --linkopt=-pg //...
```

Then run
```
bazel-bin/komfydb/komfydb
gprof bazel-bin/komfydb/komfydb > komfydb.prof
```

## Set up pre-commit

Pre-commit needs to be installed manually: 
```
pip3 install --upgrade pip
pip3 install pre-commit
pre-commit install
```
