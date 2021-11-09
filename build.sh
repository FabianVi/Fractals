cd ./builds/
cmake -G "Xcode" ..
cd ..
cmake --build ./builds/ --target Fractals -- -j 9 -DCMAKE_BUILD_TYPE=Debug