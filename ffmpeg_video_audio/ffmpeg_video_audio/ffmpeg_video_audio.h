/*包含音视频解码显示的函数*/
#include <stdio.h>
extern "C"{
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_thread.h"
};