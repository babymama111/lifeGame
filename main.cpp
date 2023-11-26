#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

void deleteArr(char **ptr, int cols);
char **firstInitMap(std::string cols, std::string rows);
void printArrAndAliveCount(char **ptr_cols, std::string cols, std::string rows);
void readSizesFromFile(std::ifstream &file, std::string &cols,
                       std::string &rows);
void secondInitMap(std::ifstream &file, char **ptr_cols);
bool livePresence(char **ptr_cols, std::string cols, std::string rows);
bool generationalCoincidence(char **ptr_cols, char **past_ptr_cols,
                             std::string cols, std::string rows);
void writePastIteration(char **ptr_cols, char **past_ptr_cols, std::string cols,
                        std::string rows);
void rearrangement(char **ptr_cols, std::string cols, std::string rows);

int main(int argc, char *argv[]) {
  std::string cols;
  std::string rows;
  std::ifstream file("start.txt");

  //читаем размеры мира  из файла
  if (file.is_open()) {
    readSizesFromFile(file, cols, rows);
  } else {
    std::cout << "Проблема с чтением файлом";
    return 0;
  }
  //создаем указатель на наше поле в нынешнем поколении
  char **ptr_cols = firstInitMap(cols, rows);
  //создаем указатель на наше поле в предыдущем поколении
  char **past_ptr_cols = firstInitMap(cols, rows);

  // подставляем живые клетки
  secondInitMap(file, ptr_cols);

  // флаг живых клеток
  bool alive = livePresence(ptr_cols, cols, rows);
  //флаг совпадения прошлого поколения
  bool constState = false;
  //переменная счетчик
  int StateCount = 1;

  // логика по следующему поколению, выводу в консоль, стирания прошлого вывода
  do {
    //		вывели
    printArrAndAliveCount(ptr_cols, cols, rows);

    //вывели поколение
    std::cout << "Поколение: " << StateCount << std::endl;
    StateCount++;

    if (constState) {
      std::cout << "Мир повторяется. Игра закончена.";
      break;
    }

    //		запись в массив на итерацию раньше
    writePastIteration(ptr_cols, past_ptr_cols, cols, rows);

    // перестановка
    rearrangement(ptr_cols, cols, rows);
    

    //флаг совпадения прошлого поколения
    constState = generationalCoincidence(ptr_cols, past_ptr_cols, cols, rows);

    if (!alive) {
      std::cout << "Нет живых клеток. Игра закончена.";
      break;
    }
    // флаг живых клеток
    alive = livePresence(ptr_cols, cols, rows);
    //задержка в секунде
    sleep(1);
    // удалить в консоли
    (void)std::system("clear");
  } while (1);

  //освобождаю массив нынешнего поколения
  deleteArr(ptr_cols, stoi(cols));
  //освобождаю массив предыдущего поколения
  deleteArr(past_ptr_cols, stoi(cols));
  //закрываю файл
  file.close();
  return 0;
}

bool generationalCoincidence(char **ptr_cols, char **past_ptr_cols,
                             std::string cols, std::string rows) {
  for (int i = 0; i < stoi(cols); i++) {
    for (int j = 0; j < stoi(rows); j++) {
      if (ptr_cols[i][j] != past_ptr_cols[i][j]) {
        return false;
      }
    }
  }
  return true;
}

void deleteArr(char **ptr, int cols) {
  for (int i = 0; i < cols; i++) {
    delete[] ptr[i];
  }
  delete[] ptr;
}

char **firstInitMap(std::string cols, std::string rows) {
  //создаю динамический двумерный массив для карты
  char **ptr_cols = new char *[stoi(cols)];
  for (int i = 0; i < stoi(cols); i++) {
    ptr_cols[i] = new char[stoi(rows)];
  }
  //инициализирую массив значениями '-'
  for (int i = 0; i < stoi(cols); i++) {
    for (int j = 0; j < stoi(rows); j++) {
      ptr_cols[i][j] = '-';
    }
  }
  return ptr_cols;
}

void printArrAndAliveCount(char **ptr_cols, std::string cols,
                           std::string rows) {
  int aliveCount = 0;
  for (int i = 0; i < stoi(cols); i++) {
    for (int j = 0; j < stoi(rows); j++) {
      std::cout << ptr_cols[i][j] << " ";
      if (ptr_cols[i][j] == '*') {
        aliveCount++;
      }
    }
    std::cout << "\n";
  }
  std::cout << "\n Живых клеток: " << aliveCount << std::endl;
}
void readSizesFromFile(std::ifstream &file, std::string &cols,
                       std::string &rows) {
  file >> cols;
  file >> rows;
}

void secondInitMap(std::ifstream &file, char **ptr_cols) {
  std::string s_i;
  std::string s_j;
  while ((file >> s_i) && (file >> s_j)) {
    int i = stoi(s_i);
    int j = stoi(s_j);
    ptr_cols[i][j] = '*';
    if (file.eof())
      break;
  }
}

bool livePresence(char **ptr_cols, std::string cols, std::string rows) {
  for (int i = 0; i < stoi(cols); i++) {
    for (int j = 0; j < stoi(rows); j++) {
      if (ptr_cols[i][j] == '*') {
        return true;
      }
    }
  }
  return false;
}

void writePastIteration(char **ptr_cols, char **past_ptr_cols, std::string cols,
                        std::string rows) {
  for (int i = 0; i < stoi(cols); i++) {
    for (int j = 0; j < stoi(rows); j++) {
      past_ptr_cols[i][j] = ptr_cols[i][j];
    }
  }
}



void rearrangement(char **ptr_cols, std::string cols, std::string rows) {
  char **new_ptr_cols = new char *[stoi(cols)];
  for (int i = 0; i < stoi(cols); i++) {
    new_ptr_cols[i] = new char[stoi(rows)];
  }
  writePastIteration(ptr_cols, new_ptr_cols, cols, rows);
  
  for (int i = 0; i < stoi(cols); i++) {
    for (int j = 0; j < stoi(rows); j++) {
      int aliveAroundCount = 0;
      int stayAliveAroundCount = 0;
      
      // Логика подсчета живых соседей
      
      for (int x = i - 1; x <= i + 1; x++) {
        for (int y = j - 1; y <= j + 1; y++) {
          if (x >= 0 && x < stoi(cols) && y >= 0 && y < stoi(rows)) {
            if (new_ptr_cols[x][y] == '*') {
              stayAliveAroundCount++;
              if (x != i || y != j) {
                aliveAroundCount++;
              }
            }
          }
        }
      }
      
      // Логика обновления состояния клетки
      if (new_ptr_cols[i][j] == '-') {
        if (stayAliveAroundCount == 3) {
          ptr_cols[i][j] = '*';
        }
      } else if (new_ptr_cols[i][j] == '*') {
        if (aliveAroundCount == 2 || aliveAroundCount == 3) {
          ptr_cols[i][j] = '*';
        } else {
          ptr_cols[i][j] = '-';
        }
      }
    }
  }
  deleteArr(new_ptr_cols, stoi(cols));
}

