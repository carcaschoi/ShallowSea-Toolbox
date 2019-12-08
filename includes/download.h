#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define GBATEMP_URL     "https://gbatemp.net/attachments/"
#define SIGS_URL        "https://gbatemp.net/threads/i-heard-that-you-guys-need-some-sweet-patches-for-atmosphere.521164/"
#define JOON_SIGS_URL   "https://api.github.com/repos/Joonie86/hekate/releases/latest"
#define JOON_HACKY_SIG  "https://github.com/Joonie86/hekate/releases/download/5.0.0J/Kosmos_patches"
#define APP_URL         "https://github.com/ITotalJustice/sigpatch-updater/releases/latest/download/sigpatch-updater.nro"
#define TEMP_FILE       "/switch/sigpatch-updater/temp"

#define ON              1
#define OFF             0

int downloadFile(const char *url, const char *output, int api);

#endif