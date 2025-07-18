#include <ncurses.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>

#define MAX_FILES 100
#define MAX_PATH 255
#define PANEL_COUNT 2
struct File {
  char name[256];
  int size;
  int isDir;
  time_t mTime;
};

struct Panel {
  struct File files[MAX_FILES];
  int file_count;
  char current_dir[MAX_PATH];
  int selected;
  WINDOW *win;
};
struct Panel panels[PANEL_COUNT];
int current_panel = 0;

void ReadDir(struct Panel *panel) {
  DIR *dir;
  struct dirent *ent;
  struct stat st;
  dir = opendir(panel->current_dir);
  panel->file_count = 0;
  while ((ent = readdir(dir)) != NULL) {
    char full_path[MAX_PATH + 256 + 2];
    snprintf(full_path, MAX_PATH + 256 + 2, "%s/%s", panel->current_dir,
             ent->d_name);
    stat(full_path, &st);
    if (!(strcmp(ent->d_name, "."))) continue;
    sprintf(panel->files[panel->file_count].name, "%s", ent->d_name);
    panel->files[panel->file_count].size = st.st_size;
    panel->files[panel->file_count].isDir = ent->d_type == DT_DIR;
    panel->files[panel->file_count].mTime = st.st_mtime;
    panel->file_count++;
  }
  closedir(dir);
}

void InitPanels() {
  for (int i = 0; i < PANEL_COUNT; i++) {
    panels[i].file_count = 0;
    strcpy(panels[i].current_dir, ".");
    panels[i].selected = 0;
    ReadDir(&panels[i]);
  }
}

void HandleInput() {
  int ch = getch();
  struct Panel *panel = &panels[current_panel];
  switch (ch) {
    case KEY_UP:
      if (panel->selected > 0) panel->selected--;
      break;
    case KEY_DOWN:
      if (panel->selected < panel->file_count - 1) panel->selected++;
      break;
    case '\n':
      if (panel->files[panel->selected].isDir) {
        char new_dir[MAX_PATH + 256 + 2];
        snprintf(new_dir, MAX_PATH + 256 + 2, "%s/%s", panel->current_dir,
                 panel->files[panel->selected].name);
        strcpy(panel->current_dir, new_dir);
        ReadDir(panel);
        panel->selected = 0;
      }
      break;
    case '\t':
      current_panel = (current_panel) ? 0 : 1;

      break;
    case 27:
    case 'q':
    case 'Q':
      endwin();
      exit(EXIT_SUCCESS);
      break;
  }
}

void DrawInterface(struct Panel *panel, int panel_num) {
  refresh();
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  int panel_width = cols / 2 - 1;
  WINDOW *win = newwin(rows, panel_width, 1, panel_num * (panel_width + 1));
  panel->win = derwin(win, rows - 2, panel_width - 2, 1, 1);
  box(win, 0, 0);

  for (int i = 0; i < panel->file_count; i++) {
    if (i == panel->selected) wattron(panel->win, A_REVERSE);
    if (panel->files[i].isDir) {
      wprintw(panel->win, "/");
    } else {
      wprintw(panel->win, " ");
    }
    wprintw(panel->win, "%-30s|%8d|", panel->files[i].name,
            panel->files[i].size);
    char date_str[20];
    struct tm *timeinfo = localtime(&panel->files[i].mTime);
    strftime(date_str, 20, "%b %d %H:%M", timeinfo);
    wprintw(panel->win, "%12s\n", date_str);
    wattroff(panel->win, A_REVERSE);
  }

  wrefresh(win);
  refresh();
  delwin(panel->win);
  delwin(win);
}

int main(int argc, char **argv) {
  initscr();
  keypad(stdscr, TRUE);
  curs_set(0);
  InitPanels();
  do {
    for (int i = 0; i < PANEL_COUNT; i++) {
      DrawInterface(&panels[i], i);
    }
    HandleInput();
  } while (1);
  endwin();
  exit(EXIT_SUCCESS);
}
