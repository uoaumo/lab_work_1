#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

const std::string kFileName = "data.txt";

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
    int stationClass = kMinClass;
};

std::string readLine()
{
    std::string line;
    if (!std::getline(std::cin, line))
    {
        std::cout << "\nInput closed. Exiting.\n";
        std::exit(0);
    }
    return line;
}

std::string readNonEmptyString(const std::string &prompt)
{
    while (true)
    {
        std::cout << prompt;
        std::string line = readLine();
        if (line.find_first_not_of(" \t") != std::string::npos)
        {
            return line;
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
        std::cout << "Error: enter an integer from " << min << " to " << max << ".\n";
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
        std::cout << "Error: enter a number from " << min << " to " << max << ".\n";
    }
}

bool readYesNo(const std::string &prompt)
{
    while (true)
    {
        std::cout << prompt << " (y/n): ";
        std::string line = readLine();
        if (line == "y" || line == "Y")
        {
            return true;
        }
        if (line == "n" || line == "N")
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
    pipe.underRepair = readYesNo("Is the pipe under repair?");
    return pipe;
}

void printPipe(const Pipe &pipe)
{
    std::cout << "--- Pipe ---\n"
              << "Name: " << pipe.name << "\n"
              << "Length: " << pipe.length << " km\n"
              << "Diameter: " << pipe.diameter << " mm\n"
              << "Under repair: " << (pipe.underRepair ? "yes" : "no") << "\n";
}

void editPipe(Pipe &pipe)
{
    pipe.underRepair = readYesNo("Is the pipe under repair?");
    std::cout << "Repair flag updated.\n";
}

void savePipe(std::ofstream &file, const Pipe &pipe)
{
    file << "PIPE\n"
         << pipe.name << "\n"
         << pipe.length << " " << pipe.diameter << " " << pipe.underRepair << "\n";
}

bool loadPipe(std::ifstream &file, Pipe &pipe)
{
    Pipe loaded;
    std::string numbers;
    int repair = 0;

    if (!std::getline(file, loaded.name) || !std::getline(file, numbers))
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

    bool valid = loaded.name.find_first_not_of(" \t") != std::string::npos &&
                 loaded.length >= kMinLengthKm && loaded.length <= kMaxLengthKm &&
                 loaded.diameter >= 1 && loaded.diameter <= kMaxDiameterMm;
    if (!valid)
    {
        return false;
    }
    pipe = loaded;
    return true;
}

CompressorStation readStation()
{
    CompressorStation station;
    station.name = readNonEmptyString("Station name: ");
    station.totalShops = readInt("Total workshops: ", 1, kMaxShops);
    station.activeShops = readInt("Running workshops: ", 0, station.totalShops);
    station.stationClass = readInt("Station class: ", kMinClass, kMaxClass);
    return station;
}

void printStation(const CompressorStation &station)
{
    std::cout << "--- Compressor station ---\n"
              << "Name: " << station.name << "\n"
              << "Total workshops: " << station.totalShops << "\n"
              << "Running workshops: " << station.activeShops << "\n"
              << "Station class: " << station.stationClass << "\n";
}

void startShop(CompressorStation &station)
{
    if (station.activeShops >= station.totalShops)
    {
        std::cout << "Error: all workshops are already running.\n";
        return;
    }
    ++station.activeShops;
    std::cout << "Workshop started.\n";
}

void stopShop(CompressorStation &station)
{
    if (station.activeShops <= 0)
    {
        std::cout << "Error: no running workshops to stop.\n";
        return;
    }
    --station.activeShops;
    std::cout << "Workshop stopped.\n";
}

void editStation(CompressorStation &station)
{
    int choice = readInt("1 - start workshop, 2 - stop workshop: ", 1, 2);
    if (choice == 1)
    {
        startShop(station);
    }
    else
    {
        stopShop(station);
    }
}

void saveStation(std::ofstream &file, const CompressorStation &station)
{
    file << "STATION\n"
         << station.name << "\n"
         << station.totalShops << " " << station.activeShops << " "
         << station.stationClass << "\n";
}

bool loadStation(std::ifstream &file, CompressorStation &station)
{
    CompressorStation loaded;
    std::string numbers;

    if (!std::getline(file, loaded.name) || !std::getline(file, numbers))
    {
        return false;
    }
    std::istringstream stream(numbers);
    if (!(stream >> loaded.totalShops >> loaded.activeShops >> loaded.stationClass))
    {
        return false;
    }
    stream >> std::ws;
    if (!stream.eof())
    {
        return false;
    }

    bool valid = loaded.name.find_first_not_of(" \t") != std::string::npos &&
                 loaded.totalShops >= 1 && loaded.totalShops <= kMaxShops &&
                 loaded.activeShops >= 0 && loaded.activeShops <= loaded.totalShops &&
                 loaded.stationClass >= kMinClass && loaded.stationClass <= kMaxClass;
    if (!valid)
    {
        return false;
    }
    station = loaded;
    return true;
}

bool saveToFile(const std::string &fileName,
                const Pipe &pipe, bool hasPipe,
                const CompressorStation &station, bool hasStation)
{
    std::ofstream file(fileName);
    if (!file)
    {
        return false;
    }
    file << std::setprecision(10);
    if (hasPipe)
    {
        savePipe(file, pipe);
    }
    if (hasStation)
    {
        saveStation(file, station);
    }
    return file.good();
}

bool loadFromFile(const std::string &fileName,
                  Pipe &pipe, bool &hasPipe,
                  CompressorStation &station, bool &hasStation)
{
    std::ifstream file(fileName);
    if (!file)
    {
        return false;
    }
    Pipe newPipe;
    CompressorStation newStation;
    bool newHasPipe = false;
    bool newHasStation = false;
    std::string header;
    while (std::getline(file, header))
    {
        if (header == "PIPE" && !newHasPipe)
        {
            if (!loadPipe(file, newPipe))
            {
                return false;
            }
            newHasPipe = true;
        }
        else if (header == "STATION" && !newHasStation)
        {
            if (!loadStation(file, newStation))
            {
                return false;
            }
            newHasStation = true;
        }
        else
        {
            return false;
        }
    }
    if (!newHasPipe && !newHasStation)
    {
        return false;
    }
    pipe = newPipe;
    hasPipe = newHasPipe;
    station = newStation;
    hasStation = newHasStation;
    return true;
}

void printMenu()
{
    std::cout << "\n===== MENU =====\n"
              << "1. Add pipe\n"
              << "2. Add compressor station\n"
              << "3. View all objects\n"
              << "4. Edit pipe\n"
              << "5. Edit compressor station\n"
              << "6. Save\n"
              << "7. Load\n"
              << "0. Exit\n";
}

int main()
{
    Pipe pipe;
    CompressorStation station;
    bool hasPipe = false;
    bool hasStation = false;

    while (true)
    {
        printMenu();
        switch (readInt("Choose an option: ", 0, 7))
        {
        case 1:
            pipe = readPipe();
            hasPipe = true;
            break;
        case 2:
            station = readStation();
            hasStation = true;
            break;
        case 3:
            if (!hasPipe && !hasStation)
            {
                std::cout << "No objects.\n";
            }
            if (hasPipe)
            {
                printPipe(pipe);
            }
            if (hasStation)
            {
                printStation(station);
            }
            break;
        case 4:
            if (hasPipe)
                editPipe(pipe);
            else
                std::cout << "Error: there is no pipe.\n";
            break;
        case 5:
            if (hasStation)
                editStation(station);
            else
                std::cout << "Error: there is no compressor station.\n";
            break;
        case 6:
            if (!hasPipe && !hasStation)
                std::cout << "Nothing to save.\n";
            else if (saveToFile(kFileName, pipe, hasPipe, station, hasStation))
                std::cout << "Saved to " << kFileName << "\n";
            else
                std::cout << "Error: could not write to " << kFileName << "\n";
            break;
        case 7:
            if (loadFromFile(kFileName, pipe, hasPipe, station, hasStation))
                std::cout << "Loaded from " << kFileName << "\n";
            else
                std::cout << "Error: file is missing or corrupted. Current data is unchanged.\n";
            break;
        case 0:
            return 0;
        }
    }
}