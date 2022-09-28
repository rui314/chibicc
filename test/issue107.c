
#include <stdio.h>
#define gettext_noop(str) (str)
#define N_(str)           gettext_noop (str)
static const char vlc_usage[] = N_(
  "Usage: %s [options] [stream] ...\n"
  "You can specify multiple streams on the commandline.\n"
  "They will be enqueued in the playlist.\n"
  "The first item specified will be played first.\n"
  "\n"
  "Options-styles:\n"
  "  --option  A global option that is set for the duration of the program.\n"
  "   -option  A single letter version of a global --option.\n"
  "   :option  An option that only applies to the stream directly before it\n"
  "            and that overrides previous settings.\n"
  "\n"
  "Stream MRL syntax:\n"
  "  [[access][/demux]://]URL[#[title][:chapter][-[title][:chapter]]]\n"
  "  [:option=value ...]\n"
  "\n"
  "  Many of the global --options can also be used as MRL specific :options.\n"
  "  Multiple :option=value pairs can be specified.\n"
  "\n"
  "URL syntax:\n"
  "  file:///path/file              Plain media file\n"
  "  http://host[:port]/file        HTTP URL\n"
  "  ftp://host[:port]/file         FTP URL\n"
  "  mms://host[:port]/file         MMS URL\n"
  "  screen://                      Screen capture\n"
  "  dvd://[device]                 DVD device\n"
  "  vcd://[device]                 VCD device\n"
  "  cdda://[device]                Audio CD device\n"
  "  udp://[[<source address>]@[<bind address>][:<bind port>]]\n"
  "                                 UDP stream sent by a streaming server\n"
  "  vlc://pause:<seconds>          Pause the playlist for a certain time\n"
  "  vlc://quit                     Special item to quit VLC\n"
  "\n");

int main(void) {
    printf("%s", vlc_usage);
    return 0;
}