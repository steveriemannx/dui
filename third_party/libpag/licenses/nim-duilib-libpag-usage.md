## Notes on nim_duilib's use of libpag
1. lz4 (the non-`lib` directory files) is not used, so nim_duilib is not subject to the GNU General Public License v2 (GPLv2);
2. The QT family of components is not used, so nim_duilib is not subject to their licenses (e.g., GNU General Public License v3 (GPLv3), etc.);

If you are concerned about the libpag license (including the main license and third-party components), you can disable PAG support through a compile switch (so libpag is not used at all).
