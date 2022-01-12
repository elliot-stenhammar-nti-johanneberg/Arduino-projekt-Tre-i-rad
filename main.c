//Import oled screen graphics library
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64);

// Define input buttons
#define down 5
#define select 6
#define right 7

// Define random variables
bool win = false;
bool draw = false;
int scoreTriangle = 0;
int scoreCross = 0;
int x_pos = 0;
int y_pos = 1;
bool triangle_active = true;
int current_pos_x = 0;
int current_pos_y = 0;

// Declare top left corner of each square in play-grid {x.pos, y.pos}
int A1_topleft[2] = {33, 0};  
int A2_topleft[2] = {54, 0};
int A3_topleft[2] = {75, 0};
int B1_topleft[2] = {33, 21};
int B2_topleft[2] = {54, 21};
int B3_topleft[2] = {75, 21};
int C1_topleft[2] = {33, 42};
int C2_topleft[2] = {54, 42};
int C3_topleft[2] = {75, 42};

// Declare top right corner of nececcary squares {x.pos, y.pos}
int B3_topright[2] = {96, 21};
int C3_topright[2] = {96, 42};

// Declare x and y coordinates for drawing triangle at specific position
int x_triangle[3] = {43, 64, 85};
int y_triangle[3] = {4, 25, 46};

// Declare x and y coordinates for drawing cross at specific position. Also used for marker pos.
int x_cross[3] = {A1_topleft[x_pos], A2_topleft[x_pos], A3_topleft[x_pos]};
int y_cross[3] = {A1_topleft[y_pos], B1_topleft[y_pos], C1_topleft[y_pos]};

// Declare state of squares: 0 = Empty, 1 = Triangle, 2 = Cross
int A1_state = 0;
int A2_state = 0;
int A3_state = 0;
int B1_state = 0;
int B2_state = 0;
int B3_state = 0;
int C1_state = 0;
int C2_state = 0;
int C3_state = 0;
int game_state[3][3]= {{A1_state, A2_state, A3_state},{B1_state, B2_state, B3_state},{C1_state, C2_state, C3_state}};

// Function for reseting game board and variables after win or draw
void resetGame() {
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 3; k++) {
      game_state[i][k] = 0;
    }
  }
  win = false;
  draw = false;
  current_pos_x = 0;
  current_pos_y = 0;
  display.clearDisplay();
  drawBoard();
}

// Function for drawing game board
void drawBoard() {
  display.clearDisplay();
  display.drawRect(A1_topleft[x_pos], A1_topleft[y_pos], display.height(), display.height(), WHITE);
  display.drawLine(A2_topleft[x_pos], A2_topleft[y_pos], A2_topleft[x_pos], display.height()-1, WHITE);
  display.drawLine(A3_topleft[x_pos], A3_topleft[y_pos], A3_topleft[x_pos], display.height()-1, WHITE);
  display.drawLine(B1_topleft[x_pos], B1_topleft[y_pos], B3_topright[x_pos], B3_topright[y_pos], WHITE);
  display.drawLine(C1_topleft[x_pos], C1_topleft[y_pos], C3_topright[x_pos], C3_topright[y_pos], WHITE);
  drawPos(0, 0);
}

// Function for drawing current score
void drawScore() {
  display.setCursor(5, 0);
  display.print(scoreTriangle);
  display.setCursor(116, 0);
  display.print(scoreCross);
}

// Function for drawing triangle
void drawTriangle(int x, int y) {
  display.fillTriangle(x, y, x-7, y+12, x+7, y+12, WHITE);
}

// Function for drawing cross
void drawCross(int x, int y) {
  display.drawLine(x, y, x+21, y+20, WHITE);
  display.drawLine(x+21, y, x, y+20, WHITE);
}

// Function for checking and updating game after button press
void updateGame() {
  if (digitalRead(select) == LOW) {
    if (game_state[current_pos_x][current_pos_y] == 0) {
      if (triangle_active == true) {
        drawTriangle(x_triangle[current_pos_x], y_triangle[current_pos_y]);
        triangle_active = false;  
      } else {
        drawCross(x_cross[current_pos_x], y_cross[current_pos_y]);
        triangle_active = true;
      }
      
    }
      update_game_state(current_pos_x, current_pos_y); 
      win_check(current_pos_x, current_pos_y);
      drawCheck();
      removePos(current_pos_x, current_pos_y);
      if (win_check(current_pos_x, current_pos_y) == false) {
        current_pos_x = 0;
        current_pos_y = 0;
        delay(200);
      drawPos(current_pos_x, current_pos_y);
    }
  }
  
  if (digitalRead(down) == LOW) {
    removePos(current_pos_x, current_pos_y);
    current_pos_y++;
    delay(200);
    drawPos(current_pos_x, current_pos_y);
  }

  if (digitalRead(right) == LOW) {
    removePos(current_pos_x, current_pos_y);
    current_pos_x++;
    delay(200);
    drawPos(current_pos_x, current_pos_y);
  }

  if (current_pos_x == 3) {
    removePos(current_pos_x, current_pos_y);
    current_pos_x = 0;
    delay(200);
    drawPos(current_pos_x, current_pos_y);
  }
  
  if (current_pos_y == 3) {
    removePos(current_pos_x, current_pos_y);
    current_pos_y = 0;
    delay(200);
    drawPos(current_pos_x, current_pos_y);
  }  
}

// Function for updating game state of selected square 
void update_game_state(int x, int y) {
  if (triangle_active != true) {
    game_state[x][y] = 1;
  } else {
    game_state[x][y] = 2;
  }
}

// Function checking for win
int win_check(int x, int y) {
  int active = game_state[x][y];
  int adjacent_counter= 0;
  // Loops column
  for(int i = 0; i < 3; i++) {
    if (game_state[x][i] == active) {
      adjacent_counter++;
    }
    if (adjacent_counter == 3) {
      win = true;
    }
  }
  
  adjacent_counter = 0;
  // Loops row
  for (int i = 0; i < 3; i++) {
    if (game_state[i][y] == active) {
      adjacent_counter++;
    }
    if (adjacent_counter == 3) {
      win = true;
    }
  };

  adjacent_counter = 0;
  // Loops diagonal (top-left to bottom-right)
   for (int i = 0; i < 3; i++) {
    if (game_state[i][i] == active) {
      adjacent_counter++;
    }
    if (adjacent_counter == 3) {
      win = true;
    }
  };
  
  adjacent_counter = 0;
  // Loops diagonal (top-right to bottom-left)
   for (int i = 0; i < 3; i++) {
    if (game_state[2-i][i] == active) {
      adjacent_counter++;
    }
    if (adjacent_counter == 3) {
      win = true;
    }
  };
  return win;
}

// Function checking for draw / full board
int drawCheck() {
  int turnsPlayed = 0;
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 3; k++) {
      if (game_state[i][k] != 0) {
        turnsPlayed++;
      }
    }
  }
  if (turnsPlayed == 9) {
    draw = true;
  } else {
    draw = false;
  }
  return draw;
}

// Function drawing marker at current position
void drawPos(int x, int y) {
  display.drawRect(x_cross[x]+1, y_cross[y]+1, 20, 20, WHITE);
}

// Function for removeing marker when you move position
void removePos(int x, int y) {
  display.drawRect(x_cross[x]+1, y_cross[y]+1, 20, 20, BLACK);
}

// Function for checking and dealing with game over 
void gameOverCheck() {
  if (draw == true && win == false) {
    resetGame();
    drawScore();
  }
  if (win == true) {
    delay(200);
    resetGame();
    if(triangle_active != true) {
      scoreTriangle++;
      drawScore();
    } else {
      scoreCross++;
      drawScore();
    };
  }
}

void setup() {
  // Initialize display and pins as well as draw board
  Serial.begin(9600);
  pinMode(down, INPUT);
  pinMode(select, INPUT);
  pinMode(right, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  drawBoard();
  display.display();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  drawScore();
}

void loop() {
  updateGame();
  gameOverCheck();
  display.display();
}