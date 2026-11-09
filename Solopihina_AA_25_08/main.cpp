#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

const double kMinLengthKm = 0.001;
const double kMaxLengthKm = 100000.0;
const int kMaxDiameterMm = 5000;
const int kMaxShops = 1000;
const int kMinClass = 1;
const int kMaxClass = 5;

struct Pipe
{
    std::string name;
    double length = 0.0;
    int diameter = 0;
    bool underRepair = false;
};

struct CompressorStation
{
    std::string name;
    int totalShops = 0;
    int activeShops = 0;
    int stationClass = 0;
};

bool isValidName(const std::string &name) { return !name.empty(); }

bool isValidPipe(const Pipe &pipe)
{
    return isValidName(pipe.name) &&
           pipe.length >= kMinLengthKm &&
           pipe.length <= kMaxLengthKm &&
           pipe.diameter > 0 && pipe.diameter <= kMaxDiameterMm;
}

bool isValidStation(const CompressorStation &station)
{
    return isValidName(station.name) && station.totalShops > 0 &&
           station.totalShops <= kMaxShops &&
           station.activeShops >= 0 &&
           station.activeShops <= station.totalShops &&
           station.stationClass >= kMinClass &&
           station.stationClass <= kMaxClass;
}

struct InputClosedError
{
};

std::string readLine()
{
    std::string line;
    if (!std::getline(std::cin, line))
    {
        throw InputClosedError();
    }
    return line;
}

std::string readNonEmptyString(const std::string &prompt)
{
    while (true)
    {
        std::cout << prompt;
        std::string value = readLine();
        if (value.find_first_not_of(" \t") != std::string::npos)
        {
            return value;
        }
        std::cout << "Error: value cannot be empty.\n";
    }
}

int readInt(const std::string &prompt, int min, int max)
{
    while (true)
    {
        std::cout << prompt;
        std::string line = readLine();
        try
        {
            size_t pos = 0;
            int value = std::stoi(line, &pos);
            if (pos == line.size() && value >= min && value <= max)
            {
                return value;
            }
        }
        catch (...)
        {
        }
        std::cout << "Error: enter an integer from "
                  << min << " to " << max << ".\n";
    }
}

double readDouble(const std::string &prompt, double min, double max)
{
    while (true)
    {
        std::cout << prompt;
        std::string line = readLine();
        try
        {
            size_t pos = 0;
            double value = std::stod(line, &pos);
            if (pos == line.size() && value >= min && value <= max)
            {
                return value;
            }
        }
        catch (...)
        {
        }
        std::cout << "Error: enter a number from "
                  << min << " to " << max << ".\n";
    }
}

bool readYesNo(const std::string &prompt)
{
    while (true)
    {
        std::cout << prompt << " (y/n): ";
        std::string answer = readLine();
        if (answer == "y" || answer == "Y")
        {
            return true;
        }
        if (answer == "n" || answer == "N")
        {
            return false;
        }
        std::cout << "Error: enter y or n.\n";
    }
}

Pipe readPipe()
{
    Pipe pipe;
    pipe.name = readNonEmptyString("Pipe name (km mark): ");
    pipe.length = readDouble("Length, km: ", kMinLengthKm, kMaxLengthKm);
    pipe.diameter = readInt("Diameter, mm: ", 1, kMaxDiameterMm);
    return pipe;
}

void printPipe(const Pipe &pipe)
{
    std::cout << "Pipe:\n"
              << "  Name:         " << pipe.name << '\n'
              << "  Length, km:   " << pipe.length << '\n'
              << "  Diameter, mm: " << pipe.diameter << '\n'
              << "  Under repair: " << (pipe.underRepair ? "yes" : "no") << '\n';
}

void editPipe(Pipe &pipe)
{
    pipe.underRepair = readYesNo("Is the pipe under repair?");
    std::cout << "Repair flag updated.\n";
}

CompressorStation readStation()
{
    CompressorStation station;
    station.name = readNonEmptyString("Station name: ");
    station.totalShops = readInt("Total workshops: ", 1, kMaxShops);
    station.activeShops = readInt("Workshops running: ", 0, station.totalShops);
    station.stationClass = readInt("Station class: ", kMinClass, kMaxClass);
    return station;
}

void printStation(const CompressorStation &station)
{
    std::cout << "Compressor station:\n"
              << "  Name:              " << station.name << '\n'
              << "  Total workshops:   " << station.totalShops << '\n'
              << "  Running workshops: " << station.activeShops << '\n'
              << "  Station class:     " << station.stationClass << '\n';
}

bool startShop(CompressorStation &station)
{
    if (station.activeShops >= station.totalShops)
        return false;
    ++station.activeShops;
    return true;
}

bool stopShop(CompressorStation &station)
{
    if (station.activeShops <= 0)
        return false;
    --station.activeShops;
    return true;
}

void editStation(CompressorStation &station)
{
    const int action = readInt("1. Start workshop\n2. Stop workshop\n0. Cancel\n> ", 0, 2);
    if (action == 0)
        return;
    const bool done = (action == 1) ? startShop(station) : stopShop(station);
    if (done)
        std::cout << "Done. Running: " << station.activeShops << '/'
                  << station.totalShops << '\n';
    else
        std::cout << (action == 1 ? "Error: all workshops are already running.\n"
                                  : "Error: no running workshops to stop.\n");
}

struct AppState
{
    Pipe pipe;
    CompressorStation station;
    bool hasPipe = false;
    bool hasStation = false;
};

void printMenu()
{
    std::cout << "\n===== MENU =====\n"
                 "1. Add pipe\n2. Add compressor station\n3. View all objects\n"
                 "4. Edit pipe\n5. Edit compressor station\n6. Save\n7. Load\n0. Exit\n";
}

void handleAddPipe(AppState &state)
{
    if (state.hasPipe && !readYesNo("A pipe already exists. Replace it?"))
        return;
    state.pipe = readPipe();
    state.hasPipe = true;
    std::cout << "Pipe added.\n";
}

void handleAddStation(AppState &state)
{
    if (state.hasStation && !readYesNo("A station already exists. Replace it?"))
        return;
    state.station = readStation();
    state.hasStation = true;
    std::cout << "Station added.\n";
}

void handleViewAll(const AppState& state)
{
    if (state.hasPipe)
    {
        printPipe(state.pipe);
    }
    if (state.hasStation)
    {
        printStation(state.station);
    }
}

void handleEditPipe(AppState &state)
{
    if (!state.hasPipe)
    {
        std::cout << "Error: there is no pipe. Add one first.\n";
        return;
    }
    editPipe(state.pipe);
}

void handleEditStation(AppState &state)
{
    if (!state.hasStation)
    {
        std::cout << "Error: there is no compressor station. Add one first.\n";
        return;
    }
    editStation(state.station);
}

const std::string kDefaultFileName = "data.txt";

bool fileExists(const std::string &fileName)
{
    std::ifstream file(fileName);
    return file.is_open();
}

bool savePipe(std::ofstream& file, const Pipe& pipe)
{
    file << "pipe\n"
         << std::setprecision(15) << pipe.name << '\n'
         << pipe.length << ' ' << pipe.diameter << ' '
         << pipe.underRepair << '\n';

    return file.good();
}

bool loadPipe(std::ifstream& file, Pipe& pipe)
{
    Pipe loaded;
    std::string numbers;
    int repair;

    if (!std::getline(file, loaded.name) ||
        !std::getline(file, numbers))
    {
        return false;
    }

    std::istringstream stream(numbers);

    if (!(stream >> loaded.length >> loaded.diameter >> repair))
    {
        return false;
    }

    stream >> std::ws;

    if (!stream.eof() || (repair != 0 && repair != 1))
    {
        return false;
    }

    loaded.underRepair = (repair == 1);

    if (!isValidPipe(loaded))
    {
        return false;
    }

    pipe = loaded;
    return true;
}

bool saveStation(std::ofstream& file, const CompressorStation& station)
{
    file << "station\n"
         << station.name << '\n'
         << station.totalShops << ' '
         << station.activeShops << ' '
         << station.stationClass << '\n';

    return file.good();
}

bool loadStation(std::ifstream& file, CompressorStation& station)
{
    CompressorStation loaded;
    std::string numbers;

    if (!std::getline(file, loaded.name) ||
        !std::getline(file, numbers))
    {
        return false;
    }

    std::istringstream stream(numbers);

    if (!(stream >> loaded.totalShops
                >> loaded.activeShops
                >> loaded.stationClass))
    {
        return false;
    }

    stream >> std::ws;

    if (!stream.eof() || !isValidStation(loaded))
    {
        return false;
    }

    station = loaded;
    return true;
}

bool saveToFile(const std::string& fileName, const AppState& state)
{
    std::ofstream file(fileName);
    if (!file)
    {
        return false;
    }
    if (state.hasPipe && !savePipe(file, state.pipe))
    {
        return false;
    }
    if (state.hasStation && !saveStation(file, state.station))
    {
        return false;
    }
    file.close();
    return !file.fail();
}

bool loadFromFile(const std::string& fileName, AppState& state)
{
    std::ifstream file(fileName);
    if (!file)
    {
        return false;
    }
    AppState loaded;
    std::string section;
    while (std::getline(file, section))
    {
        if (section == "pipe" && !loaded.hasPipe)
        {
            if (!loadPipe(file, loaded.pipe))
            {
                return false;
            }
            loaded.hasPipe = true;
        }
        else if (section == "station" && !loaded.hasStation)
        {
            if (!loadStation(file, loaded.station))
            {
                return false;
            }
            loaded.hasStation = true;
        }
        else
        {
            return false;
        }
    }
    if (file.bad())
    {
        return false;
    }
    state = loaded;
    return true;
}

std::string readFileName()
{
    std::cout << "File name (empty = " << kDefaultFileName << "): ";
    std::string name;
    if (!std::getline(std::cin, name))
        throw InputClosedError();
    return name.empty() ? kDefaultFileName : name;
}

void handleSave(const AppState &state)
{
    const std::string fileName = readFileName();
    if (!fileExists(fileName) &&
        !readYesNo("File '" + fileName + "' does not exist. Create it?"))
    {
        return;
    }
    if (saveToFile(fileName, state))
        std::cout << "Saved to '" << fileName << "'.\n";
    else
        std::cout << "Could not save '" << fileName << "'.\n";
}

void handleLoad(AppState &state)
{
    const std::string fileName = readFileName();
    if (!fileExists(fileName))
    {
        std::cout << "File '" << fileName << "' not found.\n";
    }
    else if (loadFromFile(fileName, state))
    {
        std::cout << "Loaded from '" << fileName << "'.\n";
    }
    else
    {
        std::cout << "File '" << fileName << "' has invalid data. Current data is unchanged.\n";
    }
}

int main()
{
    AppState state;
    try
    {
        while (true)
        {
            printMenu();
            switch (readInt("> ", 0, 7))
            {
            case 1:
                handleAddPipe(state);
                break;
            case 2:
                handleAddStation(state);
                break;
            case 3:
                handleViewAll(state);
                break;
            case 4:
                handleEditPipe(state);
                break;
            case 5:
                handleEditStation(state);
                break;
            case 6:
                handleSave(state);
                break;
            case 7:
                handleLoad(state);
                break;
            case 0:
                return 0;
            }
        }
    }
    catch (const InputClosedError &)
    {
        std::cout << "\nInput closed. Exiting.\n";
    }
    return 0;
}