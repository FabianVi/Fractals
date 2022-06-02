# Fractals
Fractals are a wonderful kind of art and include a fascinating implication of the mechanics of math and geometry.

## Create Video
ffmpeg -r 60 -f image2 -s 150x100 -i ./cmake-build-debug/Images/image%d.bmp -vcodec libx264 -crf 25  -pix_fmt yuv420p test.mp4
ffmpeg -r {framerate} -f image2 -s {widthxheight} -i ./cmake-build-debug/Images/image%d.bmp -vcodec libx264 -crf 25  -pix_fmt yuv420p test.mp4
ffmpeg -r 60 -f image2 -i ./cmake-build-debug/Images/image%d.bmp -vcodec libx264 -crf 1  -pix_fmt yuv420p test2.mp4