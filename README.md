# KomfyDB

Toy relational database C++ implementation based on MIT's SimpleDB.

![](/screenshot.png)

## Cloning

Run:
```
git clone --recursive https://github.com/DorosMichal/KomfyDB
```

## Requirements

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

Then, to debug run gdb on the target, e.g. to debug `//komfydb:komfydb`:
```
gdb bazel-bin/komfydb/komfydb
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
