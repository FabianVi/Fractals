cd ./builds/release/
cmake -G "Xcode" ../..
cd ../..
cmake --build ./builds/release/ --target Fractals -- -j 9 -DCMAKE_BUILD_TYPE=Debug