#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
using namespace std;

struct Pixel 
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct Position
{
    double x;
    double y;
};

class DRSTImage
{
    private:
        unsigned int blockSize;
        vector<Pixel> pixels;

        unsigned int getIndex(int imgIndex, int x, int y) const { return imgIndex * blockSize * blockSize + y * blockSize + x; }

        int getBlockNumber(int row, int col) const
        {
            if (row == 0)
                return col;

            if (row == 1)
            {
                if (col == 0)
                    return 4;

                if (col == 3)
                    return 5;
            }

            if (row == 2)
            {
                if (col == 0)
                    return 6;

                if (col == 3)
                    return 7;
            }

            if (row == 3)
                return 8 + col;

            return -1;
        }

    public:
        DRSTImage(istream& input)
        {
            char fileFormat[4];
            input.read(fileFormat, 4);

            if (fileFormat[0] != 'D' || fileFormat[1] != 'R' || fileFormat[2] != 'S' || fileFormat[3] != 'T')
            {
                blockSize = 0;
                return;
            }

            input.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
            unsigned int pixelCount = 48 * blockSize * blockSize;
            pixels.resize(pixelCount);
            input.read(reinterpret_cast<char*>(&pixels[0]), pixelCount * sizeof(Pixel));
        }

        unsigned int getImgSize() const { return 4 * blockSize; } 

        Pixel getPixel(int imgIndex, int x, int y) const
        {
            if (imgIndex < 0 || imgIndex >= 48 || x < 0 || y < 0 || x >= blockSize || y >= blockSize) 
            {
                Pixel black = {0, 0, 0};
                return black;
            }

            return pixels[getIndex(imgIndex, x, y)];
        }

        Pixel getPixel(double x, double y) const
        {
            if (x < 0 || x > 1 || y < 0 || y > 1)
            {
                Pixel black = {0, 0, 0};
                return black;
            }

            while (true)
            {
                double currentX = x;
                double currentY = y;

                for (int level = 0; level < 4; level++)
                {
                    int col = currentX * 4; 
                    int row = currentY * 4; 

                    if (col == 4)
                        col = 3;

                    if (row == 4)
                        row = 3;

                    if ((row == 1 || row == 2) && (col == 1 || col == 2))           
                    {
                        currentX = currentX * 2 - 0.5; 
                        currentY = currentY * 2 - 0.5;
                    }
                    else
                    {
                        int imgNumber = getBlockNumber(row, col);
                        int imgIndex = level * 12 + imgNumber; 

                        double imgX = currentX * 4 - col;
                        double imgY = currentY * 4 - row;

                        int pixelX = imgX * blockSize; 
                        int pixelY = imgY * blockSize;

                        if (pixelX >= (int)blockSize) 
                            pixelX = blockSize - 1;

                        if (pixelY >= (int)blockSize)
                            pixelY = blockSize - 1;

                        return getPixel(imgIndex, pixelX, pixelY);
                    }
                }

                x = currentX;
                y = currentY;
            }
        }

        void saveAsPPM(const char* filename, unsigned int size) const
        {
            ofstream writer(filename, ios::binary); 

            writer << "P6\n";
            writer << size << " " << size << "\n";
            writer << "255\n"; 

            for (int y = 0; y < size; y++) 
            {
                for (int x = 0; x < size; x++)
                {
                    double normX = (double)x / (size - 1); 
                    double normY = (double)y / (size - 1);

                    Pixel pix = getPixel(normX, normY);

                    writer.write((char*)&pix.r, 1); 
                    writer.write((char*)&pix.g, 1);
                    writer.write((char*)&pix.b, 1);
                }
            }
        }

        void saveAsPPM(const char* filename, unsigned int size, function<Position(Position)> phi) const
        {
            ofstream writer(filename, ios::binary);

            writer << "P6\n";
            writer << size << " " << size << "\n";
            writer << "255\n";

            for (int y = 0; y < size; y++)
            {
                for (int x = 0; x < size; x++)
                {
                    Position pos;
                    pos.x = (double)x / (size - 1);
                    pos.y = (double)y / (size - 1);

                    Position newPos = phi(pos);

                    Pixel pix = getPixel(newPos.x, newPos.y);

                    writer.write((char*)&pix.r, 1);
                    writer.write((char*)&pix.g, 1);
                    writer.write((char*)&pix.b, 1);
                }
            }
        }
};
