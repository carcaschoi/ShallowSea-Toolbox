#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define SIGS_URL    "http://patches.totaljustice.xyz"
#define APP_URL     "https://github.com/ITotalJustice/sigpatch-updater/releases/latest/download/sigpatch-updater.nro"

int downloadFile(const char *url, const char *output);

#endif