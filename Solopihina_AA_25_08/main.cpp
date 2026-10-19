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

Pipe readPipeFromConsole() {
    Pipe pipe;
    pipe.name = readName("Pipe name (km mark): ");
    pipe.lengthKm = readNumberInRange("Length, km: ", limits_config::kMinPipeLengthKm,
        limits_config::kMaxPipeLengthKm);
    pipe.diameterMm = readNumberInRange("Diameter, mm: ", 1, limits_config::kMaxPipeDiameterMm);
    return pipe;
}

void printPipe(const Pipe& pipe) {
    std::cout << "Pipe:\n"
        << "  Name:         " << pipe.name << '\n'
        << "  Length, km:   " << pipe.lengthKm << '\n'
        << "  Diameter, mm: " << pipe.diameterMm << '\n'
        << "  Under repair: " << (pipe.underRepair ? "yes" : "no") << '\n';
}

void editPipeRepairFlag(Pipe& pipe) {
    pipe.underRepair = readYesNo("Is the pipe under repair?");
    std::cout << "Repair flag updated.\n";
}

CompressorStation readStationFromConsole() {
    CompressorStation station;
    station.name = readName("Station name: ");
    station.workshopCount = readNumberInRange("Total workshops: ", 1, limits_config::kMaxWorkshops);
    station.workshopsRunning = readNumberInRange("Workshops running: ", 0, station.workshopCount);
    station.stationClass = readNumberInRange("Station class: ", limits_config::kMinStationClass,
        limits_config::kMaxStationClass);
    return station;
}

void printStation(const CompressorStation& station) {
    std::cout << "Compressor station:\n"
        << "  Name:              " << station.name << '\n'
        << "  Total workshops:   " << station.workshopCount << '\n'
        << "  Running workshops: " << station.workshopsRunning << '\n'
        << "  Station class:     " << station.stationClass << '\n';
}

bool startWorkshop(CompressorStation& station) {
    if (station.workshopsRunning >= station.workshopCount) return false;
    ++station.workshopsRunning;
    return true;
}

bool stopWorkshop(CompressorStation& station) {
    if (station.workshopsRunning <= 0) return false;
    --station.workshopsRunning;
    return true;
}

void editStationWorkshops(CompressorStation& station) {
    const int action = readNumberInRange("1. Start workshop\n2. Stop workshop\n0. Cancel\n> ", 0, 2);
    if (action == 0) return;
    const bool done = (action == 1) ? startWorkshop(station) : stopWorkshop(station);
    if (done)
        std::cout << "Done. Running: " << station.workshopsRunning << '/'
        << station.workshopCount << '\n';
    else
        std::cout << (action == 1 ? "Error: all workshops are already running.\n"
            : "Error: no running workshops to stop.\n");
}

int main() {
    return 0;
}