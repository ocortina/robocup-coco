/*  
 
 Copyright © 2022 DTU, Christian Andersen jcan@dtu.dk
 
 The MIT License (MIT)  https://mit-license.org/
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the “Software”), to deal in the Software without restriction, 
 including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software 
 is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies 
 or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
 FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 THE SOFTWARE. */


#include <iostream>
#include "src/ubridge.h"
#include "src/uvision.h"
#include "src/upose.h"
#include "src/ucomment.h"
#include "src/ustate.h"
#include "src/uplay.h"
#include "src/uevent.h"
#include "src/ujoy.h"

// to avoid writing std:: 
using namespace std;


bool setup(int argc, char **argv)
{ // check for command line parameters
  for (int i = 1; i < argc; i++)
  { // check for command line parameters
    // for process debug
    if (strcmp(argv[i], "help") == 0)
    { 
      printf("-----\n# User mission command line help\n");
      printf("# usage:\n#   ./user_mission [help] [ball] [show] [aruco] [videoX]\n-----\n");
      return false;
    }
  }
  // connect to robot hardware using bridge
  bridge.setup("127.0.0.1", "24001", argc, argv);
  if (true or bridge.connected)
  {  /// call setup for data structures
    pose.setup();
    comment.setup();
    state.setup();
    vision.setup(argc, argv);
    event.setup();
    joy.setup();
    printf("# Setup finished OK\n");
  }
  else
    printf("# Setup failed\n");
  sound.say("me?... I am a depressed robot.", 0.1);
//   while (sound.isSaying())
//     usleep(100000);
  // a bit of background music
  sound.play("/home/local/Music/music.mp3", 0.05);
  return true;
}

  
  void rampTask()
  {
    //sound.say(". Ramp mission.", 0.3);
    // remove old mission and clear events
    bridge.tx("regbot mclear\n");
    event.clearEvents();
    // Follow line to seesaw and go down the seesaw
    bridge.tx("regbot madd vel=0.7,  edgel=1, white=1: xl > 15\n"); 
    bridge.tx("regbot madd vel=0.3: dist=0.22\n"); 
    bridge.tx("regbot madd vel=-0.2, tr=0.1: turn=90\n"); 
    bridge.tx("regbot madd vel=0.3, edgel=0, white=1: dist=0.5\n");
    bridge.tx("regbot madd vel=0.2, edgel=0, white=1: xl > 15\n"); 
    bridge.tx("regbot madd vel=0.3, tr=0.1: turn=95\n"); 
    // Make sure to pass the two lines
    bridge.tx("regbot madd vel=0.5, edger=1,  white=1: xl > 15\n"); 
    bridge.tx("regbot madd vel=0.5: dist=0.10\n"); 
    bridge.tx("regbot madd vel=0.7, edger=0,  white=1: dist = 1.8\n"); 
    // Find way back to line for ramp
    bridge.tx("regbot madd vel=0.7: dist=1\n"); 
    bridge.tx("regbot madd vel=0.3, tr=0.1: turn=90\n"); 
    bridge.tx("regbot madd vel=0.7: dist=1\n"); 
    bridge.tx("regbot madd vel=0.3, tr=0.1: turn=-90\n"); 
    bridge.tx("regbot madd vel=0.7, white=1: xl > 15\n"); 
    bridge.tx("regbot madd vel=0.3, tr=0.1: turn=90\n"); 
    // Go to top of the ramp
    bridge.tx("regbot madd vel=0.7, edgel=1,  white=1: xl > 15\n"); 
    bridge.tx("regbot madd vel=0.3: dist=0.10\n"); 
    bridge.tx("regbot madd vel=0.7, edgel=0,  white=1: dist=1\n"); 
    // start this mission
    bridge.tx("regbot start\n");
    event.waitForEvent(0); // wait until finished
  }

void axeTask()
{
  sound.say(". Ramp mission.", 0.3);
  // remove old mission and clear events
  bridge.tx("regbot mclear\n");
  event.clearEvents();
  bridge.tx("regbot madd vel=0.0 : ir2 > 0.3\n"); //check for free passage
  bridge.tx("regbot madd : time=1\n"); //wait a little
  bridge.tx("regbot madd vel=-0.3, tr=0.1: turn=90\n"); //turn
  //bridge.tx("regbot madd vel=1, acc=3, edgel=0, white=1: xl > 15\n"); //fwd until crossing line 
  bridge.tx("regbot start\n"); // start this mission
  event.waitForEvent(0); // wait until finished
}


int main(int argc, char **argv) 
{
  if (setup(argc, argv))
  { // start mission
    std::cout << "# Robobot mission starting ...\n";
    
    rampTask();
    //axeTask();
    
    std::cout << "# Robobot mission finished ...\n";
    // remember to close camera
    vision.stop();
    sound.say("I am finished... sorry danish.", 0.2);
    while (sound.isSaying())
      sleep(1);
    bridge.tx("regbot mute 1\n");
  }
  return 0;
}
