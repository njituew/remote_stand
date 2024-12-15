#include <iostream>
#include <string>
#include <chrono>
#include <cassert>
#include <ctime>

class RemoteStand {
private:
    std::string boardName;  // Название платы
    std::chrono::system_clock::time_point freeTime;  // Время, когда стенд освободится

public:
    // Конструктор по умолчанию
    RemoteStand()
        : boardName("Unnamed Board"), freeTime(std::chrono::system_clock::now()) {}

    // Конструктор с параметрами
    RemoteStand(const std::string& board, std::chrono::system_clock::time_point time)
        : boardName(board), freeTime(time) {}

    // Конструктор копирования
    RemoteStand(const RemoteStand& other)
        : boardName(other.boardName), freeTime(other.freeTime) {}

    // Деструктор (по умолчанию)
    ~RemoteStand() {}

    // Получить название платы
    std::string getBoardName() const {
        return boardName;
    }

    // Получить время, когда стенд освободится
    std::chrono::system_clock::time_point getFreeTime() const {
        return freeTime;
    }

    // Метод для вывода информации о стенде
    void printInfo() const {
        std::time_t freeTime_t = std::chrono::system_clock::to_time_t(freeTime);
        std::cout << "Board Name: " << boardName << "\n";
        std::cout << "Free Time: " << std::ctime(&freeTime_t); // выводим время в читаемом формате
    }

    // Оператор сравнения для проверки, освобождается ли стенд раньше другого
    bool operator<(const RemoteStand& other) const {
        return freeTime < other.freeTime;
    }

    // Метод для обновления времени освобождения стенда
    void updateFreeTime(std::chrono::system_clock::time_point newTime) {
        freeTime = newTime;
    }

    // Перегрузка оператора присваивания
    RemoteStand& operator=(const RemoteStand& other) {
        if (this != &other) {
            boardName = other.boardName;
            freeTime = other.freeTime;
        }
        return *this;
    }

    // Метод для увеличения времени освобождения на заданный кулдаун
    void increaseDelay(std::chrono::minutes delay) {
        freeTime += delay;  // Увеличиваем на количество минут (или любое другое время)
    }

    // Метод для сравнения двух стендов по времени освобождения
    bool operator==(const RemoteStand& other) const {
        return freeTime == other.freeTime && boardName == other.boardName;
    }
};

// Тестирование
void testRemoteStand() {
    using namespace std::chrono;

    // Создание объектов с разным временем освобождения
    system_clock::time_point time1 = system_clock::now() + hours(2);  // Через 2 часа
    system_clock::time_point time2 = system_clock::now() + hours(1);  // Через 1 час

    RemoteStand stand1("Board A", time1);
    RemoteStand stand2("Board B", time2);
    RemoteStand stand3("Board A", time1);

    // Проверка, что названия совпадают
    assert(stand1.getBoardName() == "Board A");
    assert(stand2.getBoardName() == "Board B");

    // Проверка времени
    assert(stand1.getFreeTime() != stand2.getFreeTime());
    assert(stand1.getFreeTime() == stand3.getFreeTime());

    // Проверка оператора сравнения
    assert(stand2 < stand1); // stand2 освобождается раньше, чем stand1

    // Проверка увеличения времени освобождения на кулдаун
    std::chrono::minutes delay(30);  // 30 минут кулдауна
    stand1.increaseDelay(delay);
    assert(stand1.getFreeTime() == stand3.getFreeTime() + delay);

    // Проверка оператора присваивания
    stand2 = stand1;
    assert(stand2 == stand1); // После присваивания stand2 и stand1 должны быть равны

    std::cout << "All tests passed!\n";
}

int main() {
    // Тестирование
    testRemoteStand();

    // Создание и вывод информации о стенде
    using namespace std::chrono;

    system_clock::time_point time1 = system_clock::now() + hours(3);  // Через 3 часа
    RemoteStand stand("Board A", time1);
    stand.printInfo();

    // Обновление времени освобождения
    system_clock::time_point newTime = system_clock::now() + hours(5);
    stand.updateFreeTime(newTime);
    stand.printInfo();

    // Увеличиваем время освобождения на 30 минут
    minutes delay(30);
    stand.increaseDelay(delay);
    stand.printInfo();

    return 0;
}
