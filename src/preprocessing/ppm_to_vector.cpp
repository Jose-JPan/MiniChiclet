#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

struct PPMImage {
    int width;
    int height;
    int max_value;
    std::vector<int> pixels; // grayscale or single-channel
};

PPMImage read_ppm(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open PPM file");
    }

    std::string format;
    file >> format;
    if (format != "P3") {
        throw std::runtime_error("Only P3 PPM format supported");
    }

    PPMImage img;
    file >> img.width >> img.height;
    file >> img.max_value;

    int r, g, b;
    while (file >> r >> g >> b) {
        // Convert RGB → grayscale (simple average)
        int gray = (r + g + b) / 3;
        img.pixels.push_back(gray);
    }

    if (img.pixels.size() != static_cast<size_t>(img.width * img.height)) {
        throw std::runtime_error("Pixel count mismatch");
    }

    return img;
}

std::vector<double> normalize(const std::vector<int>& pixels, int max_value) {
    std::vector<double> out;
    out.reserve(pixels.size());

    for(int p : pixels) {
        out.push_back(static_cast<double>(p) / max_value);
    }
    return out;
}

void write_csv_row(
    std::ostream& out,
    const std::string& label,
    const std::vector<double>& data
) {
    out << label;
    for (double v : data) {
        out << "," << v;
    }
    out << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ppm_to_vector <image.ppm> <label>\n;
        return 1;
    }

    std::string ppm_file = argv[1];
    std::string label = argv[2];

    try {
        PPMImage img = read_ppm(ppm_file);
        auto vector = normalize(img.pixels, img.max_value);
        write_csv_row(std::cout, label, vector);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
