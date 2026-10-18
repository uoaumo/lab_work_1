#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

namespace limits_config {
    constexpr double kMaxPipeLengthKm = 100000.0;
    constexpr int kMaxPipeDiameterMm = 5000;
    constexpr int kMaxWorkshops = 1000;
    constexpr int kMinStationClass = 1;
    constexpr int kMaxStationClass = 5;
}  // namespace limits_config
 
struct Pipe {
    std::string name;       // километровая отметка (название)
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

bool isValidName(const std::string& name) {
    return !name.empty() && name.find('\n') == std::string::npos;
}

bool isValidPipe(const Pipe& pipe) {
    return isValidName(pipe.name) && std::isfinite(pipe.lengthKm) &&
        pipe.lengthKm > 0.0 && pipe.lengthKm <= limits_config::kMaxPipeLengthKm &&
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

int main() {
    return 0;
}