#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <map>
#include <vector>
#include <cassert>
#include <ctime>
#include <regex>
#include <thread>
#include <future>
#include <mutex>
#define DELAY std::chrono::seconds(10)
#define LOG_PATH "logs.txt"

// Абстрактный класс Stand
class Stand {
public:
    // Виртуальный деструктор, чтобы базовый класс мог быть удален через указатель на Stand
    virtual ~Stand() = default;

    // Виртуальный метод для вывода информации о стенде
    virtual void printInfo() const = 0;

    // Виртуальный метод для увеличения времени освобождения
    virtual void increaseDelay(std::chrono::seconds delay) = 0;
    virtual void increaseDelay(std::chrono::minutes delay) = 0;
    virtual void increaseDelay(std::chrono::hours delay) = 0;

    // Виртуальный метод для обновления времени освобождения
    virtual void updateFreeTime(std::chrono::system_clock::time_point newTime) = 0;
};

// Класс удалённого стенда
class RemoteStand : public Stand {
private:
    // Имя стенда
    std::string boardName;
    // Время освобождения стенда
    std::chrono::system_clock::time_point freeTime;

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

    // Деструктор
    ~RemoteStand() {}

    // Геттер название платы
    std::string getBoardName() const {
        return boardName;
    }

    // Геттер времени, когда стенд освободится
    std::chrono::system_clock::time_point getFreeTime() const {
        return freeTime;
    }

    // Сеттер для имени платы
    void setBoardName(const std::string& newBoardName) {
        boardName = newBoardName;
    }

    // Сеттер для времени когда стенд освободится
    void setFreeTime(std::chrono::system_clock::time_point newFreeTime) {
        freeTime = newFreeTime;
    }

    // Переопределение метода для вывода информации о стенде
    void printInfo() const override {
        std::time_t freeTime_t = std::chrono::system_clock::to_time_t(freeTime);

        std::cout << "Board Name: " << boardName << "\n";
        std::cout << "Free Time: " << std::ctime(&freeTime_t); // выводим время в читаемом формате
    }

    // Метод для увеличения времени освобождения (секунды)
    void increaseDelay(std::chrono::seconds delay) override {
        freeTime += delay;
    }

    // Метод для увеличения времени освобождения (минуты)
    void increaseDelay(std::chrono::minutes delay) override {
        freeTime += delay;
    }

    // Метод для увеличения времени освобождения (часы)
    void increaseDelay(std::chrono::hours delay) override {
        freeTime += delay;
    }

    // Переопределение метода для обновления времени освобождения стенда
    void updateFreeTime(std::chrono::system_clock::time_point newTime) override {
        setFreeTime(newTime);
    }

    // Оператор сравнения для проверки, освобождается ли стенд раньше другого
    bool operator<(const RemoteStand& other) const {
        return freeTime < other.freeTime;
    }

    // Оператор присваивания стендов
    RemoteStand& operator=(const RemoteStand& other) {
        if (this != &other) {
            boardName = other.boardName;
            freeTime = other.freeTime;
        }

        return *this;
    }

    // Оператор для сравнения двух стендов по времени освобождения
    bool operator==(const RemoteStand& other) const {
        return freeTime == other.freeTime && boardName == other.boardName;
    }
};

// Тесты класса удалённого стенда
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

    // Проверка сеттеров
    stand1.setBoardName("New Board A");
    assert(stand1.getBoardName() == "New Board A");

    system_clock::time_point newTime = system_clock::now() + hours(3);
    stand1.setFreeTime(newTime);
    assert(stand1.getFreeTime() == newTime);

    // Проверка оператора сравнения
    assert(stand2 < stand1); // stand2 освобождается раньше, чем stand1

    // Проверка увеличения времени освобождения на кулдаун
    std::chrono::minutes delay(30);
    stand1.increaseDelay(delay);

    // Проверяем, что время увеличилось на 30 минут
    assert(stand1.getFreeTime() == newTime + delay);

    // Проверка увеличения времени освобождения с помощью других типов времени
    std::chrono::seconds secDelay(120);  // 2 минуты в секундах
    stand1.increaseDelay(secDelay);
    assert(stand1.getFreeTime() == newTime + delay + secDelay);

    std::chrono::hours hourDelay(1);  // 1 час
    stand1.increaseDelay(hourDelay);
    assert(stand1.getFreeTime() == newTime + delay + secDelay + hourDelay);

    // Проверка оператора присваивания
    stand2 = stand1;
    // После присваивания stand2 и stand1 должны быть равны
    assert(stand2 == stand1);

    // Проверка метода обновления времени освобождения
    system_clock::time_point updatedTime = system_clock::now() + hours(4);
    stand1.updateFreeTime(updatedTime);
    assert(stand1.getFreeTime() == updatedTime);
}

// Класс кластера стендов
class StandCluster {
private:
    // Словарь название платы - вектор стендов
    std::map<std::string, std::vector<RemoteStand>> stands;

public:
    // Конструктор по умолчанию
    StandCluster() = default;

    // Конструктор копирования
    StandCluster(const StandCluster& other) : stands(other.stands) {}

    // Деструктор
    ~StandCluster() = default;

    // Метод для добавления стенда в кластер
    void addStand(const RemoteStand& stand) {
        stands[stand.getBoardName()].push_back(stand);
    }

    // Метод для удаления стенда из кластера по названию платы
    void removeStand(const std::string& boardName, const RemoteStand& stand) {
        auto it = stands.find(boardName);

        if (it != stands.end()) {
            auto& standVector = it->second;
            auto vecIt = std::remove(standVector.begin(), standVector.end(), stand);

            if (vecIt != standVector.end()) {
                standVector.erase(vecIt, standVector.end());
            }
        }
    }

    // Метод для получения всех стендов по названию платы (ссылку на вектор)
    std::vector<RemoteStand>& getStandsByBoard(const std::string& boardName) {
        return stands[boardName];
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

    // Метод для вывода количества стендов с каждой платой
    void printStandsCount() const {
        for (const auto& pair : stands) {
            std::cout << pair.first << " : " << pair.second.size() << "\n";
        }
    }

    // Оператор присваивания
    StandCluster& operator=(const StandCluster& other) {
        if (this != &other) {
            stands = other.stands;
        }

        return *this;
    }

    // Оператор сравнения (==): два кластера равны, если у них одинаковое количество стендов по каждой плате
    bool operator==(const StandCluster& other) const {
        return stands == other.stands;
    }

    // Оператор сравнения (!=): два кластера не равны, если они отличаются по количеству стендов по хотя бы одной плате
    bool operator!=(const StandCluster& other) const {
        return !(*this == other);
    }

    // Оператор сравнения (<): два кластера сравниваются по количеству стендов, можно отсортировать по количеству
    bool operator<(const StandCluster& other) const {
        if (stands.size() != other.stands.size()) {
            return stands.size() < other.stands.size();
        }

        for (const auto& pair : stands) {
            if (other.stands.find(pair.first) == other.stands.end()) {
                return false; // Если в другом кластере нет такой платы, то этот кластер "меньше"
            }

            if (pair.second.size() != other.stands.at(pair.first).size()) {
                return pair.second.size() < other.stands.at(pair.first).size();
            }
        }

        return false;
    }

    // Оператор сравнения (>): это обратный оператор по отношению к operator<
    bool operator>(const StandCluster& other) const {
        return other < *this;
    }
};

// Тесты класса кластера стендов
void testStandCluster() {
    using namespace std::chrono;

    // Создаем несколько стендов
    RemoteStand stand1("Board A", system_clock::now() + hours(1));
    RemoteStand stand2("Board A", system_clock::now() + hours(2));
    RemoteStand stand3("Board B", system_clock::now() + hours(3));
    RemoteStand stand4("Board A", system_clock::now() + hours(4));

    // Создаем кластер стендов
    StandCluster cluster;

    // Добавление стендов
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

    // Удаление стенда
    cluster.removeStand("Board A", stand1);  // Удаляем stand1 для Board A
    standsA = cluster.getStandsByBoard("Board A");

    // Проверяем, что один стенд был удален
    assert(standsA.size() == 2);  // Должно быть 2 стенда для Board A

    // Получение стендов по названию платы
    std::vector<RemoteStand> standsAAfterRemoval = cluster.getStandsByBoard("Board A");
    assert(standsAAfterRemoval.size() == 2);  // После удаления stand1 осталось 2 стенда для Board A

    // Увеличение времени освобождения для всех стендов
    minutes cooldown(30);
    cluster.increaseCooldownForAllStands("Board A", cooldown);

    // Проверяем, что время освобождения стендов увеличилось
    auto updatedStandsA = cluster.getStandsByBoard("Board A");
    auto originalFreeTime = stand2.getFreeTime();
    auto updatedFreeTime = updatedStandsA[0].getFreeTime();

    assert(updatedFreeTime == originalFreeTime + cooldown);

    // Очистка всех стендов
    cluster.clearAllStands();

    assert(cluster.getStandsByBoard("Board A").empty());  // Все стенды для Board A должны быть удалены
    assert(cluster.getStandsByBoard("Board B").empty());  // Все стенды для Board B должны быть удалены

    // Конструктор копирования
    StandCluster copiedCluster(cluster);

    assert(copiedCluster.getStandsByBoard("Board A").empty());  // Проверка, что копия пустая после очистки
    assert(copiedCluster.getStandsByBoard("Board B").empty());  // Проверка, что копия пустая после очистки

    // Оператор присваивания
    StandCluster assignedCluster;
    assignedCluster = cluster;

    // Проверка оператора присваивания
    assert(assignedCluster.getStandsByBoard("Board A").empty());
    assert(assignedCluster.getStandsByBoard("Board B").empty());

    // Оператор сравнения (==)
    StandCluster cluster2;

    // Добавление стендов в кластер
    cluster2.addStand(stand1);
    cluster2.addStand(stand2);
    cluster2.addStand(stand3);

    // Кластеры с одинаковым содержимым должны быть равны
    assert(cluster != cluster2);  // Кластеры с разным содержимым не равны
    assert(cluster == cluster);   // Кластер должен быть равен сам себе

    // Оператор сравнения (!=)
    assert(cluster != cluster2);  // Кластеры с разным содержимым не равны

    // Оператор сравнения (<)
    StandCluster cluster3;

    // Добавление стендов в кластер
    cluster3.addStand(stand1);
    cluster3.addStand(stand2);
    cluster3.addStand(stand4);
    cluster3.addStand(stand3);

    // Проверяем, что первый кластер "меньше" второго, т.к. у второго больше стендов
    assert(cluster < cluster3);

    // Оператор сравнения (>)
    assert(cluster3 > cluster);  // Проверяем, что второй кластер "больше" первого
}

// Структура для хранения о заявке
struct Request {
    std::string lastName;
    std::string firstName;
    std::string patronymic;
    std::string group;
    std::string boardName;
    std::string executablePath;
    std::string resultPath;
};

// Функция для чтения заявки из файла
Request readRequestFromFile(const std::string& fileName) {
    std::ifstream file(fileName);
    Request request;

    // Проверка открытия файла
    if (!file.is_open()) {
        std::cerr << "Ошибка при открытии файла!" << std::endl;
        return request;
    }

    // Читаем данные по очереди
    std::getline(file, request.lastName);
    std::getline(file, request.firstName);
    std::getline(file, request.patronymic);
    std::getline(file, request.group);
    std::getline(file, request.boardName);
    std::getline(file, request.executablePath);
    std::getline(file, request.resultPath);

    // Закрытие файла   
    file.close();

    return request;
}

// Функция проверки пути файла
bool isValidFilePath(const std::string& path) {
    // Простой пример проверки пути (можно улучшить под платформу)
    return !path.empty() && path.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_/\\:.") == std::string::npos;
}

// Тесты для функции проверки пути файла
void testIsValidFilePath() {
    // Тестирование валидных путей
    assert(isValidFilePath("/path/to/executable"));
    assert(isValidFilePath("C:/Users/Username/Documents/test.exe"));
    assert(isValidFilePath("valid_path/with/underscore"));
    
    // Тестирование невалидных путей
    assert(!isValidFilePath("invalid|path"));
    assert(!isValidFilePath("path with spaces"));
    assert(!isValidFilePath("<invalid>path"));
}

// Функция проверки группы
bool isValidGroup(const std::string& group) {
    // Проверка на строку, состоящую из букв (русских и латинских) и цифр
    std::regex groupPattern("^[a-zA-Zа-яА-Я0-9]+$");

    return std::regex_match(group, groupPattern);
}

// Тесты для функции проверки группы
void testIsValidGroup() {
    // Тестирование валидных групп
    assert(isValidGroup("Group123"));
    assert(isValidGroup("Group01"));
    assert(isValidGroup("12345"));
    
    // Тестирование невалидных групп
    assert(!isValidGroup("Group@123"));
    assert(!isValidGroup("Invalid Group"));
    assert(!isValidGroup("Group#123"));
}

// Функция проверки имени
bool isValidName(const std::string& name) {
    // Регулярное выражение для проверки только букв (латинских и кириллических) без пробелов
    std::regex namePattern("^[a-zA-Zа-яА-ЯёЁрРчЧьЬъЪ]+$");

    // Используем std::regex_match для проверки строки
    return std::regex_match(name, namePattern);
}

// Тесты для функции проверки имени
void testIsValidName() {
    // Тестирование валидных имен
    assert(isValidName("John"));
    assert(isValidName("Смирнов"));
    assert(isValidName("JaneDoe"));
    
    // Тестирование невалидных имен
    assert(!isValidName("John123"));
    assert(!isValidName("Invalid Name@"));
    assert(!isValidName("Иванов_Петр"));
}

// Функция для проверки файла
bool checkFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return false;
    }

    std::string line;

    // Фамилия
    std::getline(file, line);

    if (!isValidName(line)) {
        std::cerr << "Ошибка в фамилии: " << line << std::endl;
        return false;
    }

    // Имя
    std::getline(file, line);

    if (!isValidName(line)) {
        std::cerr << "Ошибка в имени: " << line << std::endl;
        return false;
    }

    // Отчество
    std::getline(file, line);

    if (!isValidName(line)) {
        std::cerr << "Ошибка в отчество: " << line << std::endl;
        return false;
    }

    // Группа
    std::getline(file, line);

    if (!isValidGroup(line)) {
        std::cerr << "Ошибка в группе: " << line << std::endl;
        return false;
    }

    // Название платы
    std::getline(file, line);

    if (line.empty()) {
        std::cerr << "Название платы не может быть пустым." << std::endl;
        return false;
    }

    // Путь к исполняемому файлу
    std::getline(file, line);

    if (!isValidFilePath(line)) {
        std::cerr << "Неверный путь к исполняемому файлу: " << line << std::endl;
        return false;
    }

    // Путь для сохранения результата
    std::getline(file, line);

    if (!isValidFilePath(line)) {
        std::cerr << "Неверный путь для сохранения результата: " << line << std::endl;
        return false;
    }

    return true;
}

// Функция для записи в логи
void writeToLog(const std::string& message) {
    std::mutex logMutex;

    // Блокируем мьютекс на время записи
    std::lock_guard<std::mutex> lock(logMutex);

    // Открываем файл для добавления данных
    std::ofstream logFile(LOG_PATH, std::ios::app);

    if (logFile.is_open()) {
        logFile << message;
    } else {
        std::cerr << "Не удалось открыть лог-файл!" << std::endl;
    }
}

// Класс для обработки заявки
class RequestProcessor {
private:
    // Кластер стендов для выбора оптимального стенда
    StandCluster& cluster;

public:
    // Конструктор
    RequestProcessor(StandCluster& cluster) : cluster(cluster) {}

    // Асинхронная функция для вывода сообщения по завершении работы
    void asyncMessage(std::chrono::system_clock::time_point freeTime, const std::string& boardName, const std::string& studentName) {
        // Рассчитываем, сколько времени осталось до времени освобождения
        auto now = std::chrono::system_clock::now();
        auto delay = freeTime - now;

        // Если стенд освободится в будущем, ждем
        if (delay > std::chrono::seconds(0)) {
            // Задержка до наступления времени
            std::this_thread::sleep_for(delay);
        }

        // Выводим сообщение, когда стенд освободится
        auto freeTime_t = std::chrono::system_clock::to_time_t(freeTime);

        std::string message = "Запрос студента " + studentName + " на стенде с платой " + boardName + " выполнено.";
        
        // Используем std::mutex для синхронизации вывода в терминал
        static std::mutex coutMutex;
        {
            // Блокируем вывод в консоль
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << message << std::endl;
        }

        // Записываем в лог
        writeToLog(message+"\n");
    }

    // Обработка заявки
    void processRequest(const Request& request) {
        // Ищем стенд с самым ранним временем освобождения для заданной платы
        auto& stands = cluster.getStandsByBoard(request.boardName);  // Получаем ссылку на вектор стендов

        // Если стенды для этой платы есть
        if (!stands.empty()) {
            // Находим стенд с минимальным временем освобождения
            auto optimalStand = std::min_element(stands.begin(), stands.end(), [](RemoteStand& a, RemoteStand& b) {
                return a.getFreeTime() < b.getFreeTime();
            });

            // Если стенд свободен, устанавливаем время освобождения на текущий момент + задержка
            auto now = std::chrono::system_clock::now();

            if (optimalStand->getFreeTime() <= now) {
                // Если стенд свободен, меняем его время освобождения
                optimalStand->updateFreeTime(now + DELAY);
            } else {
                // Если стенд занят, увеличиваем его время освобождения
                optimalStand->increaseDelay(DELAY);
            }

            // Выводим время, когда задание будет выполнено
            auto freeTime = optimalStand->getFreeTime();

            std::time_t freeTime_t = std::chrono::system_clock::to_time_t(freeTime);
            std::string message = "Задание будет выполнено на стенде с платой " + request.boardName +
                                " в " + std::ctime(&freeTime_t);
            std::cout << message;

            // Записываем в лог
            writeToLog(message);

            // Асинхронное сообщение
            std::thread(&RequestProcessor::asyncMessage, this, optimalStand->getFreeTime(), request.boardName, request.lastName).detach();
        } else {
            std::string message = "Нет доступных стендов для платы: " + request.boardName;
            std::cout << message << std::endl;

            // Записываем в лог
            writeToLog(message);
        }
    }
};

int main() {
    std::cout << "Проверка тестов перед работой..." << std::endl;
    
    // Проверка тестов перед работой
    testRemoteStand();
    testStandCluster();
    testIsValidFilePath();
    testIsValidGroup();
    testIsValidName();

    std::cout << "Тесты прошли успешно. Программа готова к использованию." << std::endl;
    
    using namespace std::chrono;

    // Создание стендов
    RemoteStand stand1("Arduino Uno", system_clock::now());
    RemoteStand stand2("Arduino Uno", system_clock::now());
    RemoteStand stand3("STM-32", system_clock::now());
    RemoteStand stand4("STM-32", system_clock::now());
    RemoteStand stand5("DE10-Lite", system_clock::now());
    RemoteStand stand6("DE10-Lite", system_clock::now());
    
    StandCluster cluster;

    // Добавление стендов в кластер
    cluster.addStand(stand1);
    cluster.addStand(stand2);
    cluster.addStand(stand3);
    cluster.addStand(stand4);
    cluster.addStand(stand5);
    cluster.addStand(stand6);

    // Вывод информации об имеющихся стендах
    std::cout << std::endl << "Стенды в кластере:" << std::endl;
    cluster.printStandsCount();

    // Отделение информации от рабочей области
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    // Создание процессор заявок
    RequestProcessor processor(cluster);
    
    // Обработка заявок
    std::cout << "Введите путь к файлу с заявкой: " << std::endl;
    while (true) {
        std::string filepath;
        std::cin >> filepath;

        if (filepath == "exit") {
            std::cout << "Выход из программы." << std::endl;
            break;
        }
        
        if (checkFile(filepath)){
            Request request = readRequestFromFile(filepath);
            processor.processRequest(request);
        }
    }

    return 0;
}
