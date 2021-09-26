#include "render_X11.hpp"

render_X11::render_X11(fifo<unsigned char *> *Input_FIFO)
{
  _videoStream = Input_FIFO;
}

void render_X11::run(void)
{
  unsigned char*  rawFrame;
  unsigned char*  canvas; // Canvas will be a pointer to where the data is stored
  XImage*         ximage;

  display = XOpenDisplay(NULL);
  visual  = DefaultVisual(display, 0);
  window  = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, XSIZE, YSIZE, 1, 0, 0);
  canvas  = (unsigned char *)malloc(XSIZE*YSIZE*4);
  ximage = XCreateImage(display, visual, DefaultDepth(display,DefaultScreen(display)), ZPixmap, 0, (char *)canvas, XSIZE, YSIZE, 32, 0);
  XMapWindow(display, window);
  XFlush(display);

  _enableStream = true;
  prevFrame = std::chrono::high_resolution_clock::now();

  while(_enableStream)
  {
    rawFrame = _videoStream->pop_last(); 
    if(rawFrame != NULL)
    {
      currFrame = std::chrono::high_resolution_clock::now();
      int prevFrameTime = (int) std::chrono::duration_cast<std::chrono::microseconds>(currFrame - prevFrame).count();
      prevFrame = currFrame;

      XDestroyImage(ximage);
      canvas = rawFrame;
      ximage = XCreateImage(display, visual, DefaultDepth(display,DefaultScreen(display)), ZPixmap, 0, (char *)canvas, XSIZE, YSIZE, 32, 0);
      XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 0, 0, XSIZE, YSIZE);
      XFlush(display);

      if(prevFrameTime < FRAMETIME)
        usleep(FRAMETIME - prevFrameTime); // Actually should correct for time used for rendering the image
    }
  }

}

void render_X11::stop(void)
{
  _enableStream = false;
}
