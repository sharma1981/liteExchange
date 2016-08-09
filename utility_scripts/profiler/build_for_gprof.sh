#/bin/bash
cd ../../build/linux/
make clean
make release_with_gprof
echo "Run ome executable at least once to create a new gnom.out for gprof"