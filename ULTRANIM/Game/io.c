#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <assert.h>
#include <sys/ioctl.h>

#include "io.h"

#define STDIN_FILENO fileno(stdin)

//
// Created by rh4618 on 11/06/19.
//

// Prints main screen game logo
void printLogo() {
  printf("---------------------------------------------------------\n");
  printf("---------------------------------------------------------\n\n");
  printf("                                            _\n");
  printf("    _    _   _     _____   ___       _     (_)\n");
  printf("   | |  | | | |   |_   _| | _ \\     / \\     _    _\n");
  printf("   | |  | | | |     | |   |  _/    // \\\\   (_)  (_)\n");
  printf("   | |  | | | |__   | |   |   \\   / ___ \\   _    _    _\n");
  printf("    \\____/   \\___|  |_|   |_|\\_\\ /_/   \\_\\ (_)  (_)  (_)\n\n");
  printf("                               _     _   _   _        _\n");
  printf("                              |  \\  | | | | |  \\    /  |\n");
  printf("                              |   \\ | | | | | |\\\\  //| |\n");
  printf("                              | |\\ \\| | | | | | \\\\// | |\n");
  printf("                              |_| \\___| |_| |_|  \\/  |_|\n\n");
  printf("---------------------------------------------------------\n");
  printf("---------------------------------------------------------\n\n");

}

void clearScreen() {
  struct winsize w;
  ioctl(fileno(stdout), TIOCGWINSZ, &w);
  unsigned int height = w.ws_row;
  printf("screenheight: %d\n", height);
  for (int i = 0; i < height; ++i) {
    printf("\n");
  }
}

int getScreenWidth() {
  struct winsize w;
  ioctl(fileno(stdout), TIOCGWINSZ, &w);
  unsigned int width = w.ws_col;
  return width;
}

int getkey() {
  struct termios settingsOld, settingsNew;
  int fetchFlag = 0;

  // Store Previous Terminal Settings
  fetchFlag = tcgetattr(STDIN_FILENO, &settingsOld);
  assert(fetchFlag == 0);

  memcpy(&settingsNew, &settingsOld, sizeof(settingsNew));

  // Disable all current terminal flags
  settingsNew.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL);
  tcsetattr(STDIN_FILENO, TCSANOW, &settingsNew);

  int c = getchar();

  // Reset all terminal flags
  fetchFlag = tcsetattr(STDIN_FILENO, TCSANOW, &settingsOld);
  assert(fetchFlag == 0);
  return c;
}

int getButtonInput() {
  int result = 0;
  while (!(result >= 1 && result <= 3)) {
    char c = getkey();
    result = c - '0';
  }
  return result;
}
