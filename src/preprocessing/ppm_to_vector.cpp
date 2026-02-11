#include <iostream>  // std::cout, std::cerr
#include <fstream>   // File I/O ( std::ifstream )
#include <vector>    // Dynamic arrays ( std::vector )
#include <string>    // std::string
#include <stdexcept> // std::runtime_error

struct PPMImage {         // Plain Data Container
    int width;               // Image Width Dimension
    int height;              // Image Height Dimension
    int max_value;           // Usually 255 in PPM 
    std::vector<int> pixels; // grayscale or single-channel → pixels flattened grayscale image
                             // Pixels are stored in 1D, not 2D
                             // Indexing would be: 
                             // pixel[y * width * x] * VERY COMMON IN ML Image Processing
};

PPMImage read_ppm(const std::string& filename) {   // Function Signature 
                                                   // Uses const reference → Avoids copying strings
    std::ifstream file(filename);        // Takes a filename
    if (!file) {                         // Checks if open failed                         
        throw std::runtime_error("Cannot open PPM file");
    }

    std::string format;
    file >> format;
    if (format != "P3") {    // P3 = ASCII PPM → Pixels are stored as text: 255 0 0 ...
        throw std::runtime_error("Only P3 PPM format supported");
    }

    PPMImage img;                   // Reading Image Metadata
    file >> img.width >> img.height;
    file >> img.max_value;

    int r, g, b;                    // Reading Pixel Data (core logic)
    while (file >> r >> g >> b) {
        // Convert RGB → grayscale (simple average)
        int gray = (r + g + b) / 3;
        img.pixels.push_back(gray);
    }

    if (img.pixels.size() != static_cast<size_t>(img.width * img.height)) {
        throw std::runtime_error("Pixel count mismatch");
    }

    return img;   // Returns a fully populated PPMImage
                  // This returns the struct by value
                  // Modern C++ uses Return Value Optimization (RVO) ⇢ No performance Penalties.
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
