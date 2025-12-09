#include <bits/stdc++.h>
#include <windows.h>
#include <conio.h>

#define end printf("\n")
using namespace std;

const int dx[] = { -1,0,0,1,-1,-1,1,1 }, dy[] = { 0,-1,1,0,-1,1,-1,1 };
int len, lon, num, x = 1, y = 1, lx = 1, ly = 1, la = 0, nu, stn[15];
int sumo = 0, sume = 0, sumt = 0, sumc = 0, nco = 1, colmuch;
uint64_t game_start_time = 0;
uint64_t tim = 0;
char mp[25][25], fil[256];
bool vis[25][25], fl[25][25];
int cell_color[25][25];

enum GameState {
	PLAYING,
	WIN,
	LOSE
};
GameState game_state = PLAYING;

const int num_color[] = { 0,1,2,3,4,5,6,7,8 };

struct word_color {
	int woc, bac;
	word_color() { woc = bac = -129; }
} col[305][3];

void setpos(int nx, int ny) {
	COORD pos;
	pos.X = ny; pos.Y = nx;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void hidden(bool now) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO pos;
	GetConsoleCursorInfo(hOut, &pos);
	pos.bVisible = now;
	SetConsoleCursorInfo(hOut, &pos);
}

void set_color_and_record(int nx, int ny, int fg, int bg) {
	WORD wColor = ((bg & 0x0F) << 4) + (fg & 0x0F);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
	cell_color[nx][ny] = (bg << 4) | fg;
}

void set_console_color(int fg, int bg) {
	WORD wColor = ((bg & 0x0F) << 4) | (fg & 0x0F);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
}

void draw_status_bar() {
	setpos(0, 0);
	int status_width = lon * 2 - 1;
	string left_num, right_num;
	int fg_color, bg_color;
	
	if (game_state == PLAYING) {
		uint64_t current_time = GetTickCount64();
		tim = (current_time - game_start_time) / 1000;
	}
	
	switch (game_state) {
	case PLAYING:
		fg_color = 0; bg_color = 9;
		left_num = to_string(nu);
		right_num = to_string(tim);
		break;
	case WIN:
		fg_color = 15; bg_color = 10;
		left_num = "0";
		right_num = to_string(tim);
		break;
	case LOSE:
		fg_color = 15; bg_color = 12;
		left_num = to_string(nu);
		right_num = to_string(tim);
		break;
	}
	
	set_console_color(fg_color, bg_color);
	
	printf("%s", left_num.c_str());
	int current_pos = (int)left_num.length();
	
	while (current_pos < status_width - (int)right_num.length()) {
		printf(" ");
		current_pos++;
	}
	
	printf("%s", right_num.c_str());
	current_pos += (int)right_num.length();
	
	set_console_color(7, 0);
	while (current_pos < status_width) {
		printf(" ");
		current_pos++;
	}
}

void read_color() {
	ifstream in("color.in");
	if (!in.is_open()) return;
	
	memset(fl, 0, sizeof(fl));
	memset(vis, 0, sizeof(vis));
	in >> colmuch;
	for (int i = 0; i < 12; i++) {
		char nch;
		in >> nch;
		for (int j = 1; j <= colmuch; j++) {
			in >> col[(unsigned char)nch][j].woc >> col[(unsigned char)nch][j].bac;
		}
	}
	in.close();
}

void print(int nx, int ny, char ch, bool is_selected) {
	int fg = 7, bg = 0;
	
	if (ch < 0) {
		ch += 128;
		fg = 4; bg = 0;
	}
	else if (ch == '0') {
		ch = ' ';
		fg = 7; bg = 0;
	}
	else if (ch == '#') {
		fg = 0; bg = 0;
	}
	else if (ch == 'F') {
		fg = 12; bg = 0;
	}
	else if (ch == '?') {
		fg = 7; bg = 0;
	}
	else if (ch >= '1' && ch <= '8') {
		int idx = ch - '0';
		fg = num_color[idx]; bg = 0;
	}
	else if (col[(unsigned char)ch][nco].bac != -129) {
		fg = col[(unsigned char)ch][nco].woc;
		bg = col[(unsigned char)ch][nco].bac;
	}
	
	if (is_selected) swap(fg, bg);
	
	set_color_and_record(nx, ny, fg, bg);
	printf("%c", ch);
	set_console_color(7, 0);
}

void redraw_cell(int nx, int ny) {
	// 地图第 1 行对应控制台第 1 行 -> setpos(nx, ...)
	setpos(nx, (ny - 1) * 2);
	bool is_selected = (nx == x && ny == y);
	if (fl[nx][ny]) print(nx, ny, 'F', is_selected);
	else if (vis[nx][ny]) print(nx, ny, mp[nx][ny], is_selected);
	else print(nx, ny, '?', is_selected);
	if (ny < lon) printf(" ");
}

void fip() {
	system("cls");
	memset(cell_color, 0, sizeof(cell_color));
	
	draw_status_bar();
	end;
	
	for (int i = 1; i <= len; i++) {
		// 地图第 i 行 -> 控制台第 i 行
		setpos(i, 0);
		for (int k = 0; k < lon * 2; k++) printf(" ");
		setpos(i, 0);
		
		for (int j = 1; j <= lon; j++) {
			redraw_cell(i, j);
		}
		end;
	}
	
	// 原来用 len+2，现在移动到 len+1（因为地图结束在 len 行）
	setpos(len + 1, 0);
	for (int k = 0; k < 50; k++) printf(" ");
	
	// 把光标移动到当前选中单元（控制台行 = x）
	setpos(x, (y - 1) * 2);
	hidden(false);
}

void open() {
	ifstream in(fil);
	if (!in.is_open()) return;
	
	memset(fl, 0, sizeof(fl));
	memset(vis, 0, sizeof(vis));
	int how;
	in >> how >> len >> lon >> num;
	if (how == 0) {
		x = 1, y = 1, lx = 1, ly = 1, la = 0, tim = 0;
		game_start_time = GetTickCount64();
		sumo = 0, sume = 0, sumt = 0, sumc = 0;
		nu = num;
		game_state = PLAYING;
	}
	for (int i = 1; i <= len; i++) {
		for (int j = 1; j <= lon; j++) {
			in >> mp[i][j];
		}
	}
	if (how == 1) {
		in >> x >> y >> lx >> ly >> la >> nu >> tim >> sumo >> sume >> sumt >> sumc;
		game_start_time = GetTickCount64() - tim * 1000;
		for (int i = 1; i <= len; i++) {
			for (int j = 1; j <= lon; j++) {
				in >> fl[i][j] >> vis[i][j];
			}
		}
	}
	in.close();
	fip();
}

void save(int ex) {
	ofstream out(fil);
	out << ex << " " << len << " " << lon << " " << num << " ";
	for (int i = 1; i <= len; i++) {
		for (int j = 1; j <= lon; j++) {
			out << mp[i][j];
		}
	}
	if (x == 1) {
		out << " " << x << " " << y << " " << lx << " " << ly << " " << la << " " << nu << " ";
		out << tim << " " << sumo << " " << sume << " " << sumt << " " << sumc << " ";
		for (int i = 1; i <= len; i++) {
			for (int j = 1; j <= lon; j++) {
				out << fl[i][j] << " " << vis[i][j] << " ";
			}
		}
	}
	out.close();
}

void outch(char ch, int x) {
	for (int i = 0; i < x; i++) printf("%c", ch);
}

char co_ar(int nx, int ny) {
	int sum = 0;
	if (mp[nx][ny] == '#') return '#';
	for (int i = 0; i < 8; i++) {
		if (mp[nx + dx[i]][ny + dy[i]] == '#') {
			sum++;
		}
	}
	return '0' + sum;
}

void start() {
	srand((unsigned)time(0));
	memset(mp, '.', sizeof(mp));
	printf("Please enter map height: "), scanf("%d", &len);
	printf("Please enter map width: "), scanf("%d", &lon);
	printf("Please enter number of mines: "), scanf("%d", &num);
	hidden(false), system("cls");
	nu = num;
	tim = 0;
	game_start_time = GetTickCount64();
	game_state = PLAYING;
	while (nu) {
		int nx = rand() % len + 1, ny = rand() % lon + 1;
		if (mp[nx][ny] == '.') {
			mp[nx][ny] = '#';
			nu--;
		}
	}
	for (int i = 1; i <= len; i++) {
		for (int j = 1; j <= lon; j++) {
			mp[i][j] = co_ar(i, j);
		}
	}
	nu = num;
	fip();
}

void dfs_x(int nx, int ny) {
	if (nx < 1 || ny < 1 || nx > len || ny > lon || mp[nx][ny] == '#' || vis[nx][ny] || fl[nx][ny]) return;
	vis[nx][ny] = 1, la++;
	redraw_cell(nx, ny);
	if (mp[nx][ny] == '0') for (int i = 0; i < 8; i++) dfs_x(nx + dx[i], ny + dy[i]);
	else if (rand() % 4 < 3) for (int i = 0; i < 4; i++) dfs_x(nx + dx[i], ny + dy[i]);
}

void finish() {
	// 原先 len+2 -> 现在 len+1
	setpos(len + 1, 0);
	for (int k = 0; k < 50; k++) printf(" ");
	setpos(len + 1, 0);
	
	int map_width = lon * 2 - 1;
	int colors[4][2] = {
		{0,8},
		{15,12},
		{0,6},
		{15,4}
	};
	string labels[4] = { "Command count:", "Error count:", "Hint count:", "Cheat count:" };
	int values[4] = { sumo, sume, sumt, sumc };
	
	for (int i = 0; i < 4; i++) {
		setpos(len + 1 + i, 0);
		set_console_color(colors[i][0], colors[i][1]);
		string line = labels[i] + " " + to_string(values[i]);
		int space_count = map_width - (int)line.length();
		if (space_count < 0) space_count = 0;
		printf("%s", line.c_str());
		for (int j = 0; j < space_count; j++) printf(" ");
		set_console_color(7, 0);
		end;
	}
	
	// 原来 len+6 -> 现在 len+5
	setpos(len + 5, 0);
	system("pause");
}

void lose() {
	game_state = LOSE;
	draw_status_bar();
	for (int i = 1; i <= len; i++) {
		for (int j = 1; j <= lon; j++) {
			redraw_cell(i, j);
		}
	}
	finish(), exit(0);
}

void update() {
	draw_status_bar();
	
	if (lx != x || ly != y) {
		redraw_cell(lx, ly);
		redraw_cell(x, y);
		lx = x, ly = y;
	}
	
	if (nu == 0 && la + num == len * lon && game_state == PLAYING) {
		game_state = WIN;
		draw_status_bar();
		finish();
		exit(0);
	}
	// 把光标放到当前选中单元（控制台行 = x）
	setpos(x, (y - 1) * 2);
}

void command();

void key(char ch) {
	if (ch == 'W' || ch == 72) x = max(x - 1, 1);
	else if (ch == 'S' || ch == 80) x = min(x + 1, len);
	else if (ch == 'A' || ch == 75) y = max(y - 1, 1);
	else if (ch == 'D' || ch == 77) y = min(y + 1, lon);
	else if (ch == 'X' || ch == ' ') {
		if (mp[x][y] == '#' && !fl[x][y]) lose();
		else dfs_x(x, y);
	}
	else if (ch == 'F') {
		if (!vis[x][y] && nu && !fl[x][y]) {
			fl[x][y] = 1;
			redraw_cell(x, y);
			nu--;
		}
	}
	else if (ch == 'E') {
		if (fl[x][y]) {
			fl[x][y] = 0;
			redraw_cell(x, y);
			nu++;
		}
	}
	else if (ch == 'C') command();
	else if (ch == 'R') {
		// 原来 len+2 -> 现在 len+1
		setpos(len + 1, 0);
		system("pause");
		setpos(len + 1, 0);
		outch(' ', 19);
	}
	// 更新光标位置
	setpos(x, (y - 1) * 2);
}

char big(char ch) {
	if (ch >= 'a' && ch <= 'z') return 'A' + ch - 'a';
	else return ch;
}

string strbig(string st) {
	string s = "";
	for (size_t i = 0; i < st.size(); i++) s += big(st[i]);
	return s;
}

string onlystr(string s) {
	string t;
	for (size_t i = 1; i < s.size(); i++) {
		if (big(s[i]) < 'A' || big(s[i]) > 'Z') {
			return t;
		}
		t += s[i];
	}
	return t;
}

void mis(string s) {
	sume++;
	// 原来 len+3 -> 现在 len+2
	setpos(len + 2, 0);
	for (int k = 0; k < 50; k++) printf(" ");
	setpos(len + 2, 0);
	
	set_color_and_record(len + 2, 0, 12, 0);
	printf("[Command Error]: %s\n", s.c_str());
	set_color_and_record(len + 2, 0, 7, 0);
	Sleep(1000), setpos(len + 2, 0);
	for (size_t i = 0; i < s.size() + 12; i++) printf(" ");
}

bool strnum(int n, string s) {
	int now = 0, tms = 1;
	memset(stn, 0, sizeof(stn));
	s += ' ';
	for (size_t i = 0; i < s.size(); i++) {
		if (s[i] == ' ') {
			if (stn[now]) stn[now] *= tms, now++, tms = 1;
		}
		else if (s[i] == '-') tms = -1;
		else if (s[i] == '+' || s[i] == '=');
		else if (s[i] >= '0' && s[i] <= '9') {
			stn[now] = stn[now] * 10 + s[i] - '0';
		}
		else {
			mis("Unknown character");
			return 1;
		}
	}
	if (now != n) {
		mis("Incorrect number of parameters");
		return 1;
	}
	return 0;
}

void order(string s) {
	int wn = 0;
	if (s.size() < 1 || s[0] != '\\' || s == "\\END") return;
	string t = onlystr(s);
	sumo++;
	if (t.size() == 1) {
		key(t[0]);
		return;
	}
	else if (t == "GOTO") wn = 2;
	else if (t == "LOOK") wn = 4;
	else if (t == "CHECK") wn = 4;
	else if (t == "TIME") wn = 1;
	else if (t == "DEBUG") wn = 0;
	else if (t == "HINT") wn = 0;
	else if (t == "SAVEMAP" || t == "SAVEPRO") wn = 0;
	else if (t == "OPEN") wn = 0;
	else if (t == "STYLE") wn = 1;
	else {
		mis("Command does not exist");
		return;
	}
	if (strnum(wn, s.substr(t.size() + 1, s.size() - t.size() - 1))) return;
	if (t == "GOTO") {
		if (stn[0] < 1 || stn[1] < 1 || stn[0] > len || stn[1] > lon) {
			mis("Parameter out of range");
			return;
		}
		x = stn[0], y = stn[1];
		update();
	}
	else if (t == "LOOK" || t == "CHECK") {
		if (stn[0] < 1 || stn[2] < 1 || stn[1] > len || stn[3] > lon) {
			mis("Parameter out of range");
			return;
		}
		// 原来 len+3 -> 现在 len+2
		setpos(len + 2, 0);
		for (int i = stn[0]; i <= stn[2]; i++) {
			for (int k = 0; k < 50; k++) printf(" ");
			end;
		}
		setpos(len + 2, 0);
		
		for (int i = stn[0]; i <= stn[2]; i++) {
			for (int j = stn[1]; j <= stn[3]; j++) {
				if (t == "LOOK") {
					print(i, j, mp[i][j], (i == x && j == y));
				}
				else {
					int sum = 0;
					for (int k = 0; k < 8; k++) {
						sum = sum + fl[i + dx[k]][j + dy[k]];
					}
					if (fl[i][j]) print(i, j, 'F', (i == x && j == y));
					else if (vis[i][j]) {
						if (sum + '0' > mp[i][j]) print(i, j, '!', (i == x && j == y));
						else print(i, j, mp[i][j] - sum, (i == x && j == y));
					}
					else print(i, j, '-', (i == x && j == y));
				}
				if (j < stn[3]) printf(" ");
			}
			end;
		}
		printf("Press any key to continue...\n"), getch();
		// 清除显示区（从 len+2 开始）
		setpos(len + 2, 0);
		for (int i = stn[0]; i <= stn[2]; i++) {
			outch(' ', (stn[3] - stn[1] + 1) * 2), end;
		}
		outch(' ', 13);
		if (t == "LOOK") sumc++;
		else sumt++;
	}
	else if (t == "TIME") {
		uint64_t ltim = tim;
		if (s[5] == '+') tim += stn[0], sumc++;
		else if (s[5] == '=') tim = stn[0], sumc++;
		else mis("Command does not exist");
		if (tim < 0) tim = ltim, mis("Data out of range");
		game_start_time = GetTickCount64() - tim * 1000;
		draw_status_bar();
	}
	else if (t == "DEBUG") {
		// 原来 len+2 -> 现在 len+1
		setpos(len + 1, 0);
		for (int k = 0; k < len + 10; k++) {
			for (int m = 0; m < 50; m++) printf(" ");
			end;
		}
		setpos(len + 1, 0);
		
		printf("X:%d,Y:%d\nTotal Mines: %d\nRemaining Mines: %d\n", x, y, num, nu);
		for (int i = 1; i <= lon; i++) {
			printf("%6d", i);
		}
		end;
		for (int i = 1; i <= len; i++) {
			printf("%-3d", i);
			for (int j = 1; j <= len; j++) {
				printf(" %d:%d:", vis[i][j], fl[i][j]);
				print(i, j, mp[i][j], (i == x && j == y));
			}
			end;
		}
		printf("Press any key to continue...");
		getch(), setpos(len + 1, 0);
		outch(' ', 5), end, outch(' ', 11), end, outch(' ', 15), end, outch(' ', 17), end, outch(' ', 60), end;
		for (int i = 1; i <= len; i++) {
			outch(' ', 6 * lon + 3), end;
		}
		outch(' ', 13), sumc++;
	}
	else if (t == "HINT") {
		int hint_x, hint_y;
		sumt++;
		// 原来 len+3 -> 现在 len+2
		setpos(len + 2, 0);
		for (int k = 0; k < 50; k++) printf(" ");
		setpos(len + 2, 0);
		
		while (1) {
			hint_x = rand() % len + 1, hint_y = rand() % lon + 1;
			if (!vis[hint_x][hint_y] && !fl[hint_x][hint_y]) {
				if (mp[hint_x][hint_y] == '#' && (rand() % 5) > 0) continue;
				printf("Cell (%d,%d) is %c", hint_x, hint_y, mp[hint_x][hint_y]);
				getch(), setpos(len + 2, 0), outch(' ', 12);
				return;
			}
		}
	}
	else if (t == "SAVEMAP") {
		// 原来 len+3 -> 现在 len+2
		setpos(len + 2, 0);
		for (int k = 0; k < 50; k++) printf(" ");
		setpos(len + 2, 0);
		
		printf("Enter file name: ");
		scanf("%s", fil);
		save(0);
		setpos(len + 2, 0), outch(' ', 12 + strlen(fil));
	}
	else if (t == "SAVEPRO") {
		// 原来 len+3 -> 现在 len+2
		setpos(len + 2, 0);
		for (int k = 0; k < 50; k++) printf(" ");
		setpos(len + 2, 0);
		
		printf("Enter file name: ");
		scanf("%s", fil);
		save(1);
		setpos(len + 2, 0), outch(' ', 12 + strlen(fil));
	}
	else if (t == "OPEN") {
		// 原来 len+3 -> 现在 len+2
		setpos(len + 2, 0);
		for (int k = 0; k < 50; k++) printf(" ");
		setpos(len + 2, 0);
		
		printf("Enter file name: ");
		scanf("%s", fil);
		open();
		setpos(len + 2, 0), outch(' ', 12 + strlen(fil));
	}
	else if (t == "STYLE") {
		if (stn[0] < 1 || stn[0] > colmuch) mis("Data out of range");
		else nco = stn[0], fip();
	}
}

void command() {
	hidden(1);
	string s = "";
	while (s != "\\END") {
		// 原来 len+2 -> 现在 len+1
		setpos(len + 1, 0);
		for (int k = 0; k < 50; k++) printf(" ");
		setpos(len + 1, 0);
		
		getline(cin, s);
		s = strbig(s);
		order(s), update();
		// 清除输入行
		setpos(len + 1, 0);
		for (size_t i = 0; i < s.size(); i++) {
			printf(" ");
		}
	}
	hidden(false);
	setpos(x, (y - 1) * 2);
}

int main() {
	SetConsoleOutputCP(CP_UTF8);
	read_color();
	start();
	
	// --- enable mouse input and disable quick edit so console doesn't pause on mouse select ---
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	DWORD prevMode = 0;
	GetConsoleMode(hIn, &prevMode);
	DWORD newMode = prevMode;
	// Enable mouse input and extended flags; disable quick edit
	newMode |= ENABLE_EXTENDED_FLAGS;
	newMode |= ENABLE_MOUSE_INPUT;
	newMode &= ~ENABLE_QUICK_EDIT_MODE;
	SetConsoleMode(hIn, newMode);
	
	INPUT_RECORD ir;
	DWORD cnt;
	
	// Main loop: keep keyboard handling via kbhit/getch, and process mouse events without consuming keyboard events.
	while (1) {
		update();
		
		// keyboard handling (keeps original behavior)
		if (kbhit()) {
			char ch = big(getch());
			key(ch);
		}
		
		// mouse handling: only read console input when the next event is a mouse event,
		// so we don't consume keyboard events from the input buffer.
		DWORD peekCount = 0;
		// Peek first event
		if (PeekConsoleInput(hIn, &ir, 1, &peekCount) && peekCount > 0) {
			// If it's a mouse event, then read and process it; otherwise leave input alone
			if (ir.EventType == MOUSE_EVENT) {
				// read that one event (consume it)
				ReadConsoleInput(hIn, &ir, 1, &cnt);
				if (ir.EventType == MOUSE_EVENT) {
					MOUSE_EVENT_RECORD mer = ir.Event.MouseEvent;
					
					// Coordinates from mer: X = column, Y = row
					int console_row = mer.dwMousePosition.Y;
					int console_col = mer.dwMousePosition.X;
					
					// Map region: status bar = row 0, map rows start at 1, each cell width = 2 chars
					// Valid console_row for map: 1..len
					// Valid console_col for map: 0 .. lon*2 - 1
					if (console_row >= 1 && console_row <= len && console_col >= 0 && console_col < lon * 2) {
						// 现在 grid row = console_row（因为第 1 行就是地图第 1 行）
						int cx = console_row;        // map row
						int cy = console_col / 2 + 1;    // map column (1-based)
						
						if (cx >= 1 && cx <= len && cy >= 1 && cy <= lon) {
							// Only update and redraw if changed
							if (x != cx || y != cy) {
								// set logical cursor to that cell
								x = cx;
								y = cy;
								// move the console cursor to the exact cell position
								setpos(x, (y - 1) * 2);
								update();
							}
						}
					}
					
					// Button handling (left / right)
					// Note: dwButtonState may contain flags for multiple buttons; test with constants.
					if (mer.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
						// left click -> open cell (same as X/space)
						if (mp[x][y] == '#' && !fl[x][y]) lose();
						else dfs_x(x, y);
					}
					else if (mer.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
						// right click -> toggle flag (same as F/E)
						if (!vis[x][y] && !fl[x][y] && nu) {
							fl[x][y] = 1;
							nu--;
						}
						else if (fl[x][y]) {
							fl[x][y] = 0;
							nu++;
						}
						redraw_cell(x, y);
					}
					// (optional) you can add double-click handling by checking mer.dwEventFlags == DOUBLE_CLICK, etc.
				}
			}
		}
		
		if (game_state != PLAYING) break;
		Sleep(10);
	}
	
	// restore previous console mode (optional)
	SetConsoleMode(hIn, prevMode);
	
	return 0;
}

