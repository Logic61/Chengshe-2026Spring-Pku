#include <fstream>
#include <iostream>
#include <cstdint>

#pragma pack(push, 1)

struct BMPFileHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BMPInfoHeader {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};

struct Pixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

#pragma pack(pop)

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "用法: rotatebmp <源文件名.bmp> <目标文件名.bmp>" << endl;
        return 1;
    }

    const char* srcFilename = argv[1];
    const char* destFilename = argv[2];

    ifstream file(srcFilename, ios::binary);
    
    if (!file) {
        cout << "无法打开源文件 " << srcFilename << "!" << endl;
        return 1;
    }

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BMPFileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BMPInfoHeader));

    int width = infoHeader.biWidth;
    int height = infoHeader.biHeight;
    int padding = (4 - (width * 3) % 4) % 4;

    Pixel** pixels = new Pixel*[height];
    for (int i = 0; i < height; ++i) {
        pixels[i] = new Pixel[width];
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            file.read(reinterpret_cast<char*>(&pixels[i][j]), sizeof(Pixel));
        }
        if (padding > 0) {
            file.seekg(padding, ios::cur);
        }
    }

    file.close();

    cout << "成功读取像素数据！" << endl;

    int newWidth = height;
    int newHeight = width;
    int newPadding = (4 - (newWidth * 3) % 4) % 4;

    Pixel** newPixels = new Pixel*[newHeight];
    for (int i = 0; i < newHeight; ++i) {
        newPixels[i] = new Pixel[newWidth];
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            newPixels[width - 1 - j][i] = pixels[i][j];
        }
    }

    infoHeader.biWidth = newWidth;
    infoHeader.biHeight = newHeight;
    infoHeader.biSizeImage = (newWidth * 3 + newPadding) * newHeight;
    fileHeader.bfSize = 54 + infoHeader.biSizeImage;

    ofstream outFile(destFilename, ios::binary);
    if (!outFile) {
        cout << "无法创建目标文件 " << destFilename << "!" << endl;
        return 1;
    }

    outFile.write(reinterpret_cast<const char*>(&fileHeader), sizeof(BMPFileHeader));
    outFile.write(reinterpret_cast<const char*>(&infoHeader), sizeof(BMPInfoHeader));

    char padByte = 0; 
    for (int i = 0; i < newHeight; ++i) {
        for (int j = 0; j < newWidth; ++j) {
            outFile.write(reinterpret_cast<const char*>(&newPixels[i][j]), sizeof(Pixel));
        }
        for (int k = 0; k < newPadding; ++k) {
            outFile.write(&padByte, 1);
        }
    }

    outFile.close();
    cout << "旋转成功！结果已保存到 " << destFilename << endl;

    for (int i = 0; i < newHeight; ++i) {
        delete[] newPixels[i];
    }
    delete[] newPixels;

    for (int i = 0; i < height; ++i) {
        delete[] pixels[i];
    }
    delete[] pixels;

    return 0;
}