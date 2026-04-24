`#include "weather_templates.hpp"

bool isNorthernStation(const Station& s) {
    return s.latitude > 42.0f;
}

bool isHighTemp(const Measurement<float>& m) {
    return m.value > 35.0f;
}

int main() {
    Station stations[4] = {
        {1, "Sofia", 42.70f, 23.32f},
        {2, "Varna", 43.21f, 27.91f},
        {3, "Kardzhali", 41.64f, 25.37f},
        {4, "Pleven", 43.41f, 24.61f}
    };

    Measurement<float> readings[5] = {
        {1, 1700000000, 34.2f},
        {2, 1700000060, 36.5f},
        {3, 1700000120, 31.0f},
        {4, 1700000180, 37.8f},
        {1, 1700000240, 33.9f}
    };

    if (!writeToFile("stations.bin", stations)) return 1;
    if (!writeToFile("readings.bin", readings)) return 1;

    std::size_t stationCount = countRecords<Station>("stations.bin");
    std::size_t readingCount = countRecords<Measurement<float>>("readings.bin");

    std::cout << "Станции във файла: " << stationCount << "\n";
    std::cout << "Измервания във файла: " << readingCount << "\n";

    if (stationCount != 4 || readingCount != 5) return 1;

    Station loadedStations[4];
    Measurement<float> loadedReadings[5];

    if (!readFromFile("stations.bin", loadedStations, 4)) return 1;
    if (!readFromFile("readings.bin", loadedReadings, 5)) return 1;

    const Station* north = findFirst(loadedStations, isNorthernStation);
    if (north) printRecord(*north);

    const Measurement<float>* hot = findFirst(loadedReadings, isHighTemp);
    if (hot) printRecord(*hot);

    return 0;
}
