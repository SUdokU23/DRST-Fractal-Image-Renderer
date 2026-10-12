#include <iostream>
#include <fstream>
#include <complex>
#include "DRSTImage.cpp"
using namespace std;

Position invert(Position pos)
{
    Position result;
    result.x = 1.0 - pos.x;
    result.y = 1.0 - pos.y;
    return result;
}

Position twist(Position pos)
{
    complex<double> z(pos.x - 0.5, pos.y - 0.5);

    z = log(z);

    complex<double> i(0, 1);

    complex<double> c = 0.915625 * exp(std::complex<double>(0, -0.41625));

    complex<double> z0(-1.1, -3.8);

    z = (z - z0) / c + z0;
    z = exp(z);

    Position result;
    result.x = z.real() + 0.5;
    result.y = z.imag() + 0.5;

    return result;
}

int main()
{
    ifstream input("cats.drst", ios::binary);

    if (!input)
    {
        cout << "Cannot open file." << endl;
        return 1;
    }

    DRSTImage img(input);

    cout << "Size of image: " << img.getImgSize() << "x" << img.getImgSize() << "\n";

    img.saveAsPPM("original.ppm", 1024);
    img.saveAsPPM("inverted.ppm", 1024, invert);
    img.saveAsPPM("twisted.ppm", 1024, twist);

    return 0;
}