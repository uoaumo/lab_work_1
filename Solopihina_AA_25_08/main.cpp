#include <fstream>
#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>
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

// Читает число в диапазоне [minValue, maxValue]
// Некорректный ввод приводит к повторному запросу
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

struct AppState {
    Pipe pipe;
    CompressorStation station;
    bool hasPipe = false;
    bool hasStation = false;
};

void printMenu() {
    std::cout << "\n===== MENU =====\n"
        "1. Add pipe\n2. Add compressor station\n3. View all objects\n"
        "4. Edit pipe\n5. Edit compressor station\n6. Save\n7. Load\n0. Exit\n";
}

void handleAddPipe(AppState& state) {
    if (state.hasPipe && !readYesNo("A pipe already exists. Replace it?")) return;
    state.pipe = readPipeFromConsole();
    state.hasPipe = true;
    std::cout << "Pipe added.\n";
}

void handleAddStation(AppState& state) {
    if (state.hasStation && !readYesNo("A station already exists. Replace it?")) return;
    state.station = readStationFromConsole();
    state.hasStation = true;
    std::cout << "Station added.\n";
}

void handleViewAll(const AppState& state) {
    if (state.hasPipe) printPipe(state.pipe);
    else std::cout << "No pipe.\n";
    if (state.hasStation) printStation(state.station);
    else std::cout << "No compressor station.\n";
}

void handleEditPipe(AppState& state) {
    if (!state.hasPipe) { std::cout << "Error: there is no pipe. Add one first.\n"; return; }
    editPipeRepairFlag(state.pipe);
}

void handleEditStation(AppState& state) {
    if (!state.hasStation) { std::cout << "Error: there is no compressor station. Add one first.\n"; return; }
    editStationWorkshops(state.station);
}

const std::string kDefaultFileName = "data.txt";

bool fileExists(const std::string& fileName) {
    std::ifstream file(fileName);
    return file.is_open();
}

// Читает строку вида "ключ: значение" и возвращает значение
bool readStringField(std::istream& in, const std::string& key, std::string& value) {
    std::string line;
    const std::string prefix = key + ": ";
    if (!std::getline(in, line) || line.compare(0, prefix.size(), prefix) != 0) return false;
    value = line.substr(prefix.size());
    return true;
}

// То же для числа, лишние символы после числа это ошибка
template <typename Number>
bool readNumberField(std::istream& in, const std::string& key, Number& value) {
    std::string text;
    if (!readStringField(in, key, text)) return false;
    std::istringstream stream(text);
    return (stream >> value) && (stream >> std::ws).eof();
}

bool savePipe(std::ostream& out, const Pipe& pipe) {
    out << "pipe\n" << std::setprecision(15)
        << "name: " << pipe.name << '\n'
        << "length: " << pipe.lengthKm << '\n'
        << "diameter: " << pipe.diameterMm << '\n'
        << "repair: " << (pipe.underRepair ? 1 : 0) << '\n';
    return static_cast<bool>(out);
}

// Вызывается после того, как строка "pipe" уже прочитана
bool loadPipe(std::istream& in, Pipe& pipe) {
    Pipe loaded;
    int repairFlag = 0;
    if (!readStringField(in, "name", loaded.name) ||
        !readNumberField(in, "length", loaded.lengthKm) ||
        !readNumberField(in, "diameter", loaded.diameterMm) ||
        !readNumberField(in, "repair", repairFlag)) {
        return false;
    }
    if (repairFlag != 0 && repairFlag != 1) return false;
    loaded.underRepair = (repairFlag == 1);
    if (!isValidPipe(loaded)) return false;
    pipe = loaded;
    return true;
}

bool saveCompressorStation(std::ostream& out, const CompressorStation& station) {
    out << "station\n"
        << "name: " << station.name << '\n'
        << "workshops: " << station.workshopCount << '\n'
        << "running: " << station.workshopsRunning << '\n'
        << "class: " << station.stationClass << '\n';
    return static_cast<bool>(out);
}

// Вызывается после того, как строка "station" уже прочитана
bool loadCompressorStation(std::istream& in, CompressorStation& station) {
    CompressorStation loaded;
    if (!readStringField(in, "name", loaded.name) ||
        !readNumberField(in, "workshops", loaded.workshopCount) ||
        !readNumberField(in, "running", loaded.workshopsRunning) ||
        !readNumberField(in, "class", loaded.stationClass)) {
        return false;
    }
    if (!isValidStation(loaded)) return false;
    station = loaded;
    return true;
}

bool saveToFile(const std::string& fileName, const AppState& state) {
    std::ofstream out(fileName);  // файл создается, если его нет
    if (!out) return false;
    if (state.hasPipe && !savePipe(out, state.pipe)) return false;
    if (state.hasStation && !saveCompressorStation(out, state.station)) return false;
    out.close();
    return !out.fail();
}

// Читает во временный AppState и при любой ошибке текущее состояние не меняется
// Пустой файл допустим
bool loadFromFile(const std::string& fileName, AppState& state) {
    std::ifstream in(fileName);
    if (!in) return false;

    AppState loaded;
    std::string section;
    while (std::getline(in, section)) {
        if (section == "pipe" && !loaded.hasPipe) {
            if (!loadPipe(in, loaded.pipe)) return false;
            loaded.hasPipe = true;
        }
        else if (section == "station" && !loaded.hasStation) {
            if (!loadCompressorStation(in, loaded.station)) return false;
            loaded.hasStation = true;
        }
        else {
            return false;  // неизвестный или повторный раздел
        }
    }
    state = loaded;
    return true;
}

std::string readFileName() {
    std::cout << "File name (empty = " << kDefaultFileName << "): ";
    std::string name;
    if (!std::getline(std::cin, name)) throw InputClosedError();
    return name.empty() ? kDefaultFileName : name;
}

void handleSave(const AppState& state) {
    const std::string fileName = readFileName();
    if (!fileExists(fileName) &&
        !readYesNo("File '" + fileName + "' does not exist. Create it?")) {
        return;
    }
    if (saveToFile(fileName, state)) std::cout << "Saved to '" << fileName << "'.\n";
    else std::cout << "Could not save '" << fileName << "'.\n";
}

void handleLoad(AppState& state) {
    const std::string fileName = readFileName();
    if (!fileExists(fileName)) {
        std::cout << "File '" << fileName << "' not found.\n";
    }
    else if (loadFromFile(fileName, state)) {
        std::cout << "Loaded from '" << fileName << "'.\n";
    }
    else {
        std::cout << "File '" << fileName << "' has invalid data. Current data is unchanged.\n";
    }
}

int main() {
    AppState state;
    try {
        while (true) {
            printMenu();
            switch (readNumberInRange("> ", 0, 7)) {
            case 1: handleAddPipe(state); break;
            case 2: handleAddStation(state); break;
            case 3: handleViewAll(state); break;
            case 4: handleEditPipe(state); break;
            case 5: handleEditStation(state); break;
            case 6: handleSave(state); break;
            case 7: handleLoad(state); break;
            case 0: return 0;
            }
        }
    }
    catch (const InputClosedError&) {
        std::cout << "\nInput closed. Exiting.\n";
    }
    return 0;
}