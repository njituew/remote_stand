#include <iostream>
#include <string>
#include <chrono>
#include <map>
#include <vector>
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
void test1() {
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

class StandCluster {
private:
    // Словарь название платы - вектор стендов
    std::map<std::string, std::vector<RemoteStand>> stands;

public:
    // Конструктор по умолчанию
    StandCluster() = default;

    // Конструктор копирования
    StandCluster(const StandCluster& other) {
        stands = other.stands;  // Копируем весь контейнер
    }

    // Деструктор
    ~StandCluster() = default;

    // Оператор присваивания
    StandCluster& operator=(const StandCluster& other) {
        if (this != &other) {
            stands = other.stands;
        }
        return *this;
    }

    // Метод для добавления стенда в кластер
    void addStand(const RemoteStand& stand) {
        stands[stand.getBoardName()].push_back(stand);
    }

    // Метод для удаления стенда из кластера по названию платы
    void removeStand(const std::string& boardName, const RemoteStand& stand) {
        auto it = stands.find(boardName);
        if (it != stands.end()) {
            auto& standVector = it->second;
            for (auto vecIt = standVector.begin(); vecIt != standVector.end(); ++vecIt) {
                if (*vecIt == stand) {
                    standVector.erase(vecIt);
                    break;
                }
            }
        }
    }

    // Метод для получения всех стендов по названию платы
    std::vector<RemoteStand> getStandsByBoard(const std::string& boardName) const {
        auto it = stands.find(boardName);
        if (it != stands.end()) {
            return it->second;
        }
        return {};  // Возвращаем пустой вектор, если не нашли
    }

    // Метод для увеличения времени освобождения всех стендов на заданный кулдаун
    void increaseCooldownForAllStands(const std::string& boardName, std::chrono::minutes delay) {
        auto it = stands.find(boardName);
        if (it != stands.end()) {
            for (auto& stand : it->second) {
                stand.increaseDelay(delay);
            }
        }
    }

    // Метод для очистки всех стендов в кластере
    void clearAllStands() {
        stands.clear();
    }

    // Метод для вывода всех стендов в кластере
    void printAllStands() const {
        for (const auto& pair : stands) {
            std::cout << "Board: " << pair.first << "\n";
            for (const auto& stand : pair.second) {
                stand.printInfo();
            }
        }
    }
};

void test2() {
    using namespace std::chrono;

    // Создаем несколько стендов
    RemoteStand stand1("Board A", system_clock::now() + hours(1));
    RemoteStand stand2("Board A", system_clock::now() + hours(2));
    RemoteStand stand3("Board B", system_clock::now() + hours(3));
    RemoteStand stand4("Board A", system_clock::now() + hours(4));

    // Создаем кластер стендов
    StandCluster cluster;

    // Тест 1: Добавление стендов
    cluster.addStand(stand1);
    cluster.addStand(stand2);
    cluster.addStand(stand3);
    cluster.addStand(stand4);

    // Проверяем, что стенды добавлены
    auto standsA = cluster.getStandsByBoard("Board A");
    auto standsB = cluster.getStandsByBoard("Board B");

    // Проверяем, что для Board A 3 стенда, для Board B - 1 стенд
    assert(standsA.size() == 3);  // Должно быть 3 стенда для Board A
    assert(standsB.size() == 1);  // Должно быть 1 стенд для Board B

    // Тест 2: Удаление стенда
    cluster.removeStand("Board A", stand1);  // Удаляем stand1 для Board A
    standsA = cluster.getStandsByBoard("Board A");

    // Проверяем, что один стенд был удален
    assert(standsA.size() == 2);  // Должно быть 2 стенда для Board A

    // Тест 3: Получение стендов по названию платы
    std::vector<RemoteStand> standsAAfterRemoval = cluster.getStandsByBoard("Board A");
    assert(standsAAfterRemoval.size() == 2);  // После удаления stand1 осталось 2 стенда для Board A

    // Тест 4: Увеличение времени освобождения для всех стендов
    minutes cooldown(30);  // Кулдаун 30 минут
    cluster.increaseCooldownForAllStands("Board A", cooldown);

    // Проверяем, что время освобождения стендов увеличилось
    auto updatedStandsA = cluster.getStandsByBoard("Board A");
    auto originalFreeTime = stand2.getFreeTime();
    auto updatedFreeTime = updatedStandsA[0].getFreeTime();
    assert(updatedFreeTime == originalFreeTime + cooldown);  // Время должно быть увеличено на 30 минут

    // Тест 5: Очистка всех стендов
    cluster.clearAllStands();
    assert(cluster.getStandsByBoard("Board A").empty());  // Все стенды для Board A должны быть удалены
    assert(cluster.getStandsByBoard("Board B").empty());  // Все стенды для Board B должны быть удалены

    // Тест 6: Конструктор копирования
    StandCluster copiedCluster(cluster);
    assert(copiedCluster.getStandsByBoard("Board A").size() == 0);  // Проверка, что копия пустая после очистки
    assert(copiedCluster.getStandsByBoard("Board B").size() == 0);  // Проверка, что копия пустая после очистки

    // Тест 7: Оператор присваивания
    StandCluster assignedCluster;
    assignedCluster = cluster;
    assert(assignedCluster.getStandsByBoard("Board A").size() == 0);  // Проверка оператора присваивания
    assert(assignedCluster.getStandsByBoard("Board B").size() == 0);  // Проверка оператора присваивания

    std::cout << "All tests passed!" << std::endl;
}

int main() {
    test1();
    test2();
    std::cout << "Тесты прошли, работаем." << std::endl;
    
    using namespace std::chrono;

    RemoteStand stand1("Arduino Uno", system_clock::now());
    RemoteStand stand2("Arduino Uno", system_clock::now());
    RemoteStand stand3("STM-32", system_clock::now());
    RemoteStand stand4("STM-32", system_clock::now());
    RemoteStand stand5("DE10-Lite", system_clock::now());
    RemoteStand stand6("DE10-Lite", system_clock::now());
    
    StandCluster cluster;
    cluster.addStand(stand1);
    cluster.addStand(stand2);
    cluster.addStand(stand3);
    cluster.addStand(stand4);
    cluster.addStand(stand5);
    cluster.addStand(stand6);
    cluster.printAllStands();

    return 0;
}
