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

struct PipelineState {
    std::optional<Pipe> pipe;
    std::optional<CompressorStation> station;
};

void printMenu() {
    std::cout << "\n===== MENU =====\n"
        "1. Add pipe\n2. Add compressor station\n3. View all objects\n"
        "4. Edit pipe\n5. Edit compressor station\n6. Save\n7. Load\n0. Exit\n";
}

void handleAddPipe(PipelineState& state) {
    if (state.pipe && !readYesNo("A pipe already exists. Replace it?")) return;
    state.pipe = readPipeFromConsole();
    std::cout << "Pipe added.\n";
}

void handleAddStation(PipelineState& state) {
    if (state.station && !readYesNo("A station already exists. Replace it?")) return;
    state.station = readStationFromConsole();
    std::cout << "Station added.\n";
}

void handleViewAll(const PipelineState& state) {
    if (state.pipe) printPipe(*state.pipe);
    else std::cout << "No pipe.\n";
    if (state.station) printStation(*state.station);
    else std::cout << "No compressor station.\n";
}

void handleEditPipe(PipelineState& state) {
    if (!state.pipe) { std::cout << "Error: there is no pipe. Add one first.\n"; return; }
    editPipeRepairFlag(*state.pipe);
}

void handleEditStation(PipelineState& state) {
    if (!state.station) { std::cout << "Error: there is no compressor station. Add one first.\n"; return; }
    editStationWorkshops(*state.station);
}

const std::string kFileHeader = "PIPELINE_DATA_V1";
const std::string kDefaultFileName = "data.txt";

void writePipe(std::ostream& out, const Pipe& pipe) {
    out << pipe.name << '\n' << pipe.lengthKm << ' ' << pipe.diameterMm << ' '
        << pipe.underRepair << '\n';
}

std::optional<Pipe> readPipeFromStream(std::istream& in) {
    Pipe pipe;
    if (!std::getline(in >> std::ws, pipe.name)) return std::nullopt;
    if (!(in >> pipe.lengthKm >> pipe.diameterMm >> pipe.underRepair)) return std::nullopt;
    if (!isValidPipe(pipe)) return std::nullopt;
    return pipe;
}

void writeStation(std::ostream& out, const CompressorStation& station) {
    out << station.name << '\n' << station.workshopCount << ' '
        << station.workshopsRunning << ' ' << station.stationClass << '\n';
}

std::optional<CompressorStation> readStationFromStream(std::istream& in) {
    CompressorStation station;
    if (!std::getline(in >> std::ws, station.name)) return std::nullopt;
    if (!(in >> station.workshopCount >> station.workshopsRunning >> station.stationClass))
        return std::nullopt;
    if (!isValidStation(station)) return std::nullopt;
    return station;
}

bool saveState(const PipelineState& state, const std::string& fileName) {
    std::ofstream out(fileName);
    if (!out) return false;
    out << kFileHeader << '\n';
    if (state.pipe) { out << "PIPE\n"; writePipe(out, *state.pipe); }
    else out << "NOPIPE\n";
    if (state.station) { out << "STATION\n"; writeStation(out, *state.station); }
    else out << "NOSTATION\n";
    return static_cast<bool>(out);
}

// Данные читаются во временный объект: при ошибке текущее состояние не меняется.
std::optional<PipelineState> loadState(const std::string& fileName) {
    std::ifstream in(fileName);
    std::string header, marker;
    if (!(in >> header) || header != kFileHeader) return std::nullopt;

    PipelineState loaded;
    if (!(in >> marker)) return std::nullopt;
    if (marker == "PIPE") {
        loaded.pipe = readPipeFromStream(in);
        if (!loaded.pipe) return std::nullopt;
    }
    else if (marker != "NOPIPE") {
        return std::nullopt;
    }

    if (!(in >> marker)) return std::nullopt;
    if (marker == "STATION") {
        loaded.station = readStationFromStream(in);
        if (!loaded.station) return std::nullopt;
    }
    else if (marker != "NOSTATION") {
        return std::nullopt;
    }
    return loaded;
}

std::string readFileName() {
    std::cout << "File name (empty = " << kDefaultFileName << "): ";
    std::string name;
    if (!std::getline(std::cin, name)) throw InputClosedError();
    return name.empty() ? kDefaultFileName : name;
}

void handleSave(const PipelineState& state) {
    const std::string fileName = readFileName();
    if (saveState(state, fileName)) std::cout << "Saved to '" << fileName << "'.\n";
    else std::cout << "Error: could not write file '" << fileName << "'.\n";
}

void handleLoad(PipelineState& state) {
    const std::string fileName = readFileName();
    auto loaded = loadState(fileName);
    if (!loaded) {
        std::cout << "Error: could not read '" << fileName
            << "' (missing or invalid data). Current data is unchanged.\n";
        return;
    }
    state = *loaded;
    std::cout << "Loaded from '" << fileName << "'.\n";
}

int main() {
    PipelineState state;
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