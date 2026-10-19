#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

namespace limits_config {
    constexpr double kMinPipeLengthKm = 0.001;
    constexpr double kMaxPipeLengthKm = 100000.0;
    constexpr int kMaxPipeDiameterMm = 5000;
    constexpr int kMaxWorkshops = 1000;
    constexpr int kMinStationClass = 1;
    constexpr int kMaxStationClass = 5;
}  // namespace limits_config
 
struct Pipe {
    std::string name;       // километровая отметка, название
    double lengthKm = 0.0;  // длина, км
    int diameterMm = 0;     // диаметр, мм
    bool underRepair = false;
};

struct CompressorStation {
    std::string name;
    int workshopCount = 0;     // всего цехов
    int workshopsRunning = 0;  // цехов в работе
    int stationClass = 0;      // класс станции
};

bool isValidName(const std::string& name) { return !name.empty(); }

bool isValidPipe(const Pipe& pipe) {
    return isValidName(pipe.name) &&
        pipe.lengthKm >= limits_config::kMinPipeLengthKm &&
        pipe.lengthKm <= limits_config::kMaxPipeLengthKm &&
        pipe.diameterMm > 0 && pipe.diameterMm <= limits_config::kMaxPipeDiameterMm;
}

bool isValidStation(const CompressorStation& station) {
    return isValidName(station.name) && station.workshopCount > 0 &&
        station.workshopCount <= limits_config::kMaxWorkshops &&
        station.workshopsRunning >= 0 &&
        station.workshopsRunning <= station.workshopCount &&
        station.stationClass >= limits_config::kMinStationClass &&
        station.stationClass <= limits_config::kMaxStationClass;
}

struct InputClosedError {};

// Читает число в диапазоне [minValue, maxValue].
// Некорректный ввод приводит к повторному запросу.
template <typename Number>
Number readNumberInRange(const std::string& prompt, Number minValue, Number maxValue) {
    while (true) {
        std::cout << prompt;
        Number value{};
        const bool readOk = static_cast<bool>(std::cin >> value);
        if (!readOk && std::cin.eof()) throw InputClosedError();
        std::cin.clear();
        const bool lineIsClean = readOk && std::cin.peek() == '\n';  // нет лишних символов
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (lineIsClean && value >= minValue && value <= maxValue) return value;
        std::cout << "Error: enter a number from " << minValue << " to " << maxValue << ".\n";
    }
}

// Читает непустую строку
std::string readName(const std::string& prompt) {
    std::cout << prompt;
    std::string name;
    if (!std::getline(std::cin >> std::ws, name)) throw InputClosedError();
    return name;
}

bool readYesNo(const std::string& prompt) {
    while (true) {
        const std::string answer = readName(prompt + " (y/n): ");
        if (answer == "y" || answer == "Y") return true;
        if (answer == "n" || answer == "N") return false;
        std::cout << "Error: enter 'y' or 'n'.\n";
    }
}

int main() {
    return 0;
}